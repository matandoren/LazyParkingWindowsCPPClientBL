#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <atomic>
#include <list>
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#include "SocketReader.h"
#include "ReplyMessageParser.h"
#include "SocketWriter.h"
#include "RequestSerializer.h"
#include "BlockingQueue.h"
#include "Event.h"

#define DEFAULT_PORT "7000"
#define DEFAULT_IP "localhost"
#define DEFAULT_NUMBER_OF_PARKING_SPOTS (2 * (19 + 14))

namespace LazyParkingWindowsCPPClientBL
{
    class Model
    {
    private:
        struct ExpiredUserRecord
        {
            std::string m_Username;
            time_t m_ExpirationDate;
        };

        struct ParkingSpotStatus // data structure used for caching parking spot statuses received from the server
        {
            int m_ID; // parking spots IDs start from 1
            bool m_IsValid; // is this cached status up to date
            bool m_IsOccupied;
            bool m_IsReserved;
            std::string m_ReservedFor;
        };

        static int s_InstanceCount;

        std::thread* m_WritingThread;
        std::atomic<bool> m_IsConnected;
        std::atomic<bool> m_IsConnecting;
        std::atomic<bool> m_IsDisconnecting;
        SOCKET m_ConnectSocket;
        addrinfo* m_AddressInfoResult;
        std::string m_IP;
        std::string m_Port;
        BlockingQueue<Request> m_RequestsQueue;
        int m_NumberOfParkingSpots;
        ParkingSpotStatus* m_CachedStatuses;
        std::list<ExpiredUserRecord> m_ExpiredUsersList;
        int m_ExpiredUsersListSize;

    public:
        Event<Model, int/* i_PermissionLevel */, bool/* i_UsernameNotFound */, bool/* i_WrongPW */> LoginReply;
        Event<Model, int/* i_ParkingSpotID */, bool/* i_IsOccupied */, bool/* i_IsReserved */, const std::string&/* i_ReservedFor */> ParkingStatusUpdate;
        Event<Model> LogoutReply;
        Event<Model, bool/* i_IsSuccessful */> OpenGateReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_IDNotFound */> CancelReservationReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_UsernameNotFound */> ChangePasswordReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_UsernameNotFound */> DeleteDriverReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_DateExpired */, const std::string&/* i_Username */, int/* i_CardKey */> AddDriverReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_UsernameNotFound */, bool/* i_DateExpired */> UpdateDriverExpirationReply;
        Event<Model, bool/* i_NoPermission */, bool/* i_IDNotFound */, bool/* i_DateExpired */> ReserveParkingSpotReply;
        Event<Model, int/* i_NumberOfExpiredUsers */> RequestExpiredUsersReply;

        Model();
        Model(const Model&) = delete;
        ~Model();
        // the following methods can be called only while NOT logged in; calling any of these methods while logged in will result in the methods doing nothing but returning 'false'
        bool SetNumberOfParkingSpots(int i_numberOfParkingSpots);
        bool SetIP(std::string i_IP);
        bool SetPort(std::string i_Port);
        bool Login(const std::string& i_Username, const std::string& i_Password); // asynchronous method; the result is provided via the LoginReply event

        // the following methods can be called only while logged in; calling any of these methods while NOT logged in will result in the methods doing nothing but returning 'false'
        bool Logout(); // asynchronous method; the result is provided via the LogoutReply event
        bool OpenGate(); // asynchronous method; the result is provided via the OpenGateReply event
        bool CancelReservation(int i_ParkingSpotID); // asynchronous method; the result is provided via the CancelReservationReply event; i_ParkingSpotID must be greater than 0
        bool ChangePassword(const std::string& i_Username, const std::string& i_Password); // asynchronous method; the result is provided via the ChangePasswordReply event; both username and password must not be empty
        bool DeleteDriver(const std::string& i_Username); // asynchronous method; the result is provided via the DeleteDriverReply event
        bool AddDriver(const std::string& i_Name, const std::string& i_Password, time_t i_ExpirationDate/* NULL_DATE for no expiration */); // asynchronous method; the result is provided via the AddDriverReply event
        bool UpdateDriverExpiration(const std::string& i_Username, time_t i_ExpirationDate/* NULL_DATE for no expiration */); // asynchronous method; the result is provided via the UpdateDriverExpirationReply event
        bool ReserveParkingSpot(int i_ParkingSpotID, const std::string& i_ReservedFor, time_t i_ExpirationDate/* NULL_DATE for no expiration */); // asynchronous method; the result is provided via the ReserveParkingSpotReply event
        bool RequestExpiredUsers(); // asynchronous method; the result is provided via the RequestExpiredUsersReply event
        bool GetNextExpiredUser(std::string& o_Username, time_t& o_ExpirationDate); // first call RequestExpiredUsers; after the RequestExpiredUsersReply event occurs, use this method to get expired users
        bool GetParkingSpotStatus(int i_ID, bool& o_IsOccupied, bool& o_IsReserved, std::string& o_ReservedFor);

    private:
        void writingThreadRun();
        void readingThreadRun();
        void requestNextExpiredUser();
        void connectSocket();
        void handleLoginReply(const ReplyMessage& i_Reply);
        void handleAddDriverReply(const ReplyMessage& i_Reply);
        void handleCancelReservationReply(const ReplyMessage& i_Reply);
        void handleChangeExpirationReply(const ReplyMessage& i_Reply);
        void handleChangePWReply(const ReplyMessage& i_Reply);
        void handleGetNextExpiredReply(const ReplyMessage& i_Reply);
        void handleGetParkingStatusReply(const ReplyMessage& i_Reply, const ReplyMessage& i_LatestLoginReply);
        void handleOpenGateReply(const ReplyMessage& i_Reply);
        void handleRemoveDriverReply(const ReplyMessage& i_Reply);
        void handleLogoutReply(const ReplyMessage& i_LatestLoginReply);
        void handleReserveParkingSpotReply(const ReplyMessage& i_Reply);
    };
}