#include "pch.h"
#include <regex>
#include "Model.h"
#include "Exceptions.h"

using namespace LazyParkingWindowsCPPClientBL;

int Model::s_InstanceCount = 0;

Model::Model(): m_IP(DEFAULT_IP), m_Port(DEFAULT_PORT)
{
    if (s_InstanceCount == 0)
    {
        // Initialize Winsock
        WSADATA wsaData;
        int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (errorCode != NO_ERROR)
        {
            throw WSAStartupException(errorCode);
        }
    }

    s_InstanceCount++;
    m_WritingThread = nullptr;
    m_CachedStatuses = nullptr;
    m_ConnectSocket = INVALID_SOCKET;
    m_ExpiredUsersListSize = 0;
    m_NumberOfParkingSpots = 0;
    SetNumberOfParkingSpots(DEFAULT_NUMBER_OF_PARKING_SPOTS);
}

Model::~Model()
{
    if (m_IsConnected || m_IsConnecting)
    {
        Logout();
    }

    if (m_WritingThread != nullptr)
    {
        m_WritingThread->join();
        delete m_WritingThread;
    }

    delete[] m_CachedStatuses;
    s_InstanceCount--;
    if (s_InstanceCount == 0)
    {
        WSACleanup();
    }
}

bool Model::SetNumberOfParkingSpots(int i_numberOfParkingSpots)
{
    if (m_IsConnected || m_IsConnecting || i_numberOfParkingSpots <= 0)
    {
        return false;
    }

    if (i_numberOfParkingSpots != m_NumberOfParkingSpots)
    {
        if (m_NumberOfParkingSpots > 0)
        {
            delete[] m_CachedStatuses;
        }

        m_NumberOfParkingSpots = i_numberOfParkingSpots;
        m_CachedStatuses = new ParkingSpotStatus[m_NumberOfParkingSpots];
        for (int i = 0; i < m_NumberOfParkingSpots; i++)
        {
            m_CachedStatuses[i].m_ID = i + 1;
            m_CachedStatuses[i].m_IsValid = false;
        }
    }

    return true;
}

bool Model::SetIP(std::string i_IP)
{
    if (m_IsConnected || m_IsConnecting)
    {
        return false;
    }

    m_IP = std::move(i_IP);
    return true;
}

bool Model::SetPort(std::string i_Port)
{
    if (m_IsConnected || m_IsConnecting)
    {
        return false;
    }

    m_Port = std::move(i_Port);
    return true;
}

bool Model::Login(const std::string& i_Username, const std::string& i_Password)
{
    if (m_IsConnecting || (m_IsConnected && !m_IsDisconnecting) || i_Username.size() == 0 || i_Password.size() == 0)
    {
        return false;
    }

    m_IsConnecting = true;
    if (m_WritingThread != nullptr)
    {
        m_WritingThread->join();
        delete m_WritingThread;
    }

    m_RequestsQueue.Clear();
    connectSocket();
    Request request;
    request.SetType(eRequestType::Login);
    request.SetStringField1(i_Username);
    request.SetStringField2(i_Password);
    m_RequestsQueue.Enqueue(request);
    m_WritingThread = new std::thread(&Model::writingThreadRun, this);
    return true;
}

bool Model::Logout()
{
    if ((!m_IsConnected && !m_IsConnecting) || m_IsDisconnecting)
    {
        return false;
    }

    m_IsDisconnecting = true;
    Request request;
    request.SetType(eRequestType::Logout);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::OpenGate()
{
    if (!m_IsConnected || m_IsDisconnecting)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::OpenGate);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::CancelReservation(int i_ParkingSpotID)
{
    if (!m_IsConnected || m_IsDisconnecting)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::CancelReservation);
    request.SetIntField(i_ParkingSpotID);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::ChangePassword(const std::string& i_Username, const std::string& i_Password)
{
    if (!m_IsConnected || m_IsDisconnecting || i_Password.empty())
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::ChangePW);
    request.SetStringField1(i_Username);
    request.SetStringField2(i_Password);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::DeleteDriver(const std::string& i_Username)
{
    if (!m_IsConnected || m_IsDisconnecting)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::RemoveDriver);
    request.SetStringField1(i_Username);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::AddDriver(const std::string& i_Name, const std::string& i_Password, time_t i_ExpirationDate)
{
    int firstNameLength = 0, numOfSpaces = 0;

    if (!m_IsConnected || m_IsDisconnecting || i_Password.empty() || i_ExpirationDate < 0)
    {
        return false;
    }

    if (!std::regex_match(i_Name.cbegin(), i_Name.cend(), std::regex("[A-Za-z]{2,}[ ]+[A-Za-z]{2,}")))
    {
        return false;
    }

    // check that the combined length of the first and last names is at most 20 
    while (i_Name[firstNameLength] != ' ')
    {
        firstNameLength++;
    }

    while (i_Name[firstNameLength + numOfSpaces] == ' ')
    {
        numOfSpaces++;
    }

    if (firstNameLength + (i_Name.length() - (firstNameLength + numOfSpaces)) > 20)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::AddDriver);
    request.SetStringField1(i_Name);
    request.SetStringField2(i_Password);
    request.SetDateField(i_ExpirationDate);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::UpdateDriverExpiration(const std::string& i_Username, time_t i_ExpirationDate)
{
    if (!m_IsConnected || m_IsDisconnecting || i_ExpirationDate < 0)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::ChangeExpiration);
    request.SetStringField1(i_Username);
    request.SetDateField(i_ExpirationDate);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::ReserveParkingSpot(int i_ParkingSpotID, const std::string& i_ReservedFor, time_t i_ExpirationDate)
{
    if (!m_IsConnected || m_IsDisconnecting || i_ExpirationDate < 0)
    {
        return false;
    }

    Request request;
    request.SetType(eRequestType::ReserveParkingSpot);
    request.SetIntField(i_ParkingSpotID);
    request.SetStringField1(i_ReservedFor);
    request.SetDateField(i_ExpirationDate);
    m_RequestsQueue.Enqueue(request);
    return true;
}

bool Model::RequestExpiredUsers()
{
    if (!m_IsConnected || m_IsDisconnecting)
    {
        return false;
    }

    m_ExpiredUsersList.clear();
    m_ExpiredUsersListSize = 0;
    requestNextExpiredUser();
    return true;
}

bool Model::GetNextExpiredUser(std::string& o_Username, time_t& o_ExpirationDate)
{
    if (!m_IsConnected || m_IsDisconnecting || m_ExpiredUsersList.empty())
    {
        return false;
    }

    ExpiredUserRecord expiredUser = m_ExpiredUsersList.front();
    o_Username = std::move(expiredUser.m_Username);
    o_ExpirationDate = expiredUser.m_ExpirationDate;
    m_ExpiredUsersList.pop_front();
    m_ExpiredUsersListSize--;
    return true;
}

bool Model::GetParkingSpotStatus(int i_ID, bool& o_IsOccupied, bool& o_IsReserved, std::string& o_ReservedFor)
{
    if (!m_IsConnected || m_IsDisconnecting || i_ID < 1 || i_ID > m_NumberOfParkingSpots || !m_CachedStatuses[i_ID - 1].m_IsValid)
    {
        return false;
    }

    o_IsOccupied = m_CachedStatuses[i_ID - 1].m_IsOccupied;
    o_IsReserved = m_CachedStatuses[i_ID - 1].m_IsReserved;
    o_ReservedFor = m_CachedStatuses[i_ID - 1].m_ReservedFor;
    return true;
}

void Model::writingThreadRun()
{
    SocketWriter writer(m_ConnectSocket);
    RequestSerializer serializer(writer);
    Request request;
    
    std::thread readingThread(&Model::readingThreadRun, this);
    do
    {
        request = m_RequestsQueue.Dequeue();
        serializer.WriteRequest(request);
    } while (request.GetType() != eRequestType::Logout);

    readingThread.join();
    closesocket(m_ConnectSocket);
    freeaddrinfo(m_AddressInfoResult);
    m_IsConnected = false;
    m_IsDisconnecting = false;
}

void Model::readingThreadRun()
{
    SocketReader reader(m_ConnectSocket);
    ReplyMessageParser parser(reader);
    ReplyMessage reply, latestLoginReply;
    eRequestType requestType;

    do
    {
        reply = parser.GetReply();
        requestType = reply.GetType();
        switch (requestType)
        {
        case eRequestType::Login:
            latestLoginReply = reply;
            handleLoginReply(reply);
            break;
        case eRequestType::AddDriver:
            handleAddDriverReply(reply);
            break;
        case eRequestType::CancelReservation:
            handleCancelReservationReply(reply);
            break;
        case eRequestType::ChangeExpiration:
            handleChangeExpirationReply(reply);
            break;
        case eRequestType::ChangePW:
            handleChangePWReply(reply);
            break;
        case eRequestType::GetNextExpired:
            handleGetNextExpiredReply(reply);
            break;
        case eRequestType::GetParkingStatus:
            handleGetParkingStatusReply(reply, latestLoginReply);
            break;
        case eRequestType::OpenGate:
            handleOpenGateReply(reply);
            break;
        case eRequestType::RemoveDriver:
            handleRemoveDriverReply(reply);
            break;
        case eRequestType::ReserveParkingSpot:
            handleReserveParkingSpotReply(reply);
            break;
        default: // eRequestType::Logout:
            handleLogoutReply(latestLoginReply);
            break;
        }
    } while (requestType != eRequestType::Logout);
}

void Model::handleLoginReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    if (reply == eReply::Success)
    {
        for (int i = 0; i < m_NumberOfParkingSpots; i++)
        {
            m_CachedStatuses[i].m_IsValid = false;
            Request request;
            request.SetType(eRequestType::GetParkingStatus);
            request.SetIntField(i + 1);
            m_RequestsQueue.Enqueue(request);
        }
    }
    else
    {
        Logout();
    }
}

void Model::handleAddDriverReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    AddDriverReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::DateExpired, i_Reply.GetStringField(), i_Reply.GetIntField());
}

void Model::handleCancelReservationReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    CancelReservationReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::IDNotFound);
}

void Model::handleChangeExpirationReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    UpdateDriverExpirationReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::UsernameNotFound, reply == eReply::DateExpired);
}

void Model::handleChangePWReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    ChangePasswordReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::UsernameNotFound);
}

void Model::handleGetNextExpiredReply(const ReplyMessage& i_Reply)
{
    if (i_Reply.GetReply() == eReply::Success)
    {
        ExpiredUserRecord expiredUser;

        expiredUser.m_Username = i_Reply.GetStringField();
        expiredUser.m_ExpirationDate = i_Reply.GetDateField();
        m_ExpiredUsersList.push_back(expiredUser);
        m_ExpiredUsersListSize++;
        requestNextExpiredUser();
    }
    else
    {
        RequestExpiredUsersReply.Invoke(m_ExpiredUsersListSize);
    }
}

void Model::handleGetParkingStatusReply(const ReplyMessage& i_Reply, const ReplyMessage& i_LatestLoginReply)
{
    int index = i_Reply.GetIntField() - 1;

    if (index >= 0 && index < m_NumberOfParkingSpots)
    {
        m_CachedStatuses[index].m_IsOccupied = i_Reply.GetBoolField1();
        m_CachedStatuses[index].m_IsReserved = i_Reply.GetBoolField2();
        m_CachedStatuses[index].m_ReservedFor = i_Reply.GetStringField();
        m_CachedStatuses[index].m_IsValid = true;
        if (m_IsConnected)
        {
            ParkingStatusUpdate.Invoke(m_CachedStatuses[index].m_ID, m_CachedStatuses[index].m_IsOccupied, m_CachedStatuses[index].m_IsReserved, m_CachedStatuses[index].m_ReservedFor);
        }
        else
        {
            bool isAllValid = true;

            for (int i = 0; isAllValid && i < m_NumberOfParkingSpots; i++)
            {
                isAllValid &= m_CachedStatuses[i].m_IsValid;
            }

            if (isAllValid)
            {
                eReply reply = i_LatestLoginReply.GetReply();

                m_IsConnected = true;
                m_IsConnecting = false;
                LoginReply.Invoke(i_LatestLoginReply.GetIntField(), reply == eReply::UsernameNotFound, reply == eReply::WrongPW);
            }
        }
    }
}

void Model::handleOpenGateReply(const ReplyMessage& i_Reply)
{
    OpenGateReply.Invoke(i_Reply.GetReply() == eReply::Success);
}

void Model::handleRemoveDriverReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    DeleteDriverReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::UsernameNotFound);
}

void Model::handleLogoutReply(const ReplyMessage& i_LatestLoginReply)
{
    if (m_IsConnected)
    {
        LogoutReply.Invoke();
    }
    else
    {
        eReply reply = i_LatestLoginReply.GetReply();

        m_IsConnecting = false;
        LoginReply.Invoke(i_LatestLoginReply.GetIntField(), reply == eReply::UsernameNotFound, reply == eReply::WrongPW);
    }
}

void Model::handleReserveParkingSpotReply(const ReplyMessage& i_Reply)
{
    eReply reply = i_Reply.GetReply();

    ReserveParkingSpotReply.Invoke(reply == eReply::InsufficientPriviledge, reply == eReply::IDNotFound, reply == eReply::DateExpired);
}

void Model::requestNextExpiredUser()
{
    Request request;
    request.SetType(eRequestType::GetNextExpired);
    m_RequestsQueue.Enqueue(request);
}

void Model::connectSocket()
{
    // Create a SOCKET for connecting to server
    m_ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_ConnectSocket == INVALID_SOCKET) {
        m_IsConnecting = false;
        throw SocketConnectionException(std::string("Error at socket(): ") + std::to_string(WSAGetLastError()));
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    addrinfo* tempNode = NULL, hints;
    m_AddressInfoResult = NULL;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int iResult = getaddrinfo(m_IP.c_str(), m_Port.c_str(), &hints, &m_AddressInfoResult);
    if (iResult != 0) {
        m_IsConnecting = false;
        throw SocketConnectionException(std::string("getaddrinfo failed: ") + std::to_string(iResult));
    }
    //----------------------
    // Connect to server.
    tempNode = m_AddressInfoResult;
    do
    {
        iResult = connect(m_ConnectSocket, tempNode->ai_addr, tempNode->ai_addrlen);
        tempNode = tempNode->ai_next;
    } while (iResult == SOCKET_ERROR && tempNode != NULL);

    if (iResult == SOCKET_ERROR) {
        closesocket(m_ConnectSocket);
        freeaddrinfo(m_AddressInfoResult);
        m_IsConnecting = false;
        throw SocketConnectionException(std::string("Unable to connect to server: ") + std::to_string(WSAGetLastError()));
    }
}