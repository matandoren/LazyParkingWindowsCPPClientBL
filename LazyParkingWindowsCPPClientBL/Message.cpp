#include "pch.h"
#include <string>
#include <unordered_map>
#include "Message.h"

using namespace LazyParkingWindowsCPPClientBL;

Request::Request(): m_StringField1(), m_StringField2()
{
	SetType(eRequestType::Logout);
	SetIntField(0);
	SetDateField(NULL_DATE);
}

eRequestType Request::GetType() const
{
	return m_Type;
}

void Request::SetType(eRequestType i_Type)
{
	m_Type = i_Type;
}

int Request::GetIntField() const
{
	return m_IntField;
}

void Request::SetIntField(int i_IntField)
{
	m_IntField = i_IntField;
}

string Request::GetStringField1() const
{
	return m_StringField1;
}

void Request::SetStringField1(const string& i_StringField1)
{
	m_StringField1 = i_StringField1;
}

void Request::SetStringField1(string&& i_StringField1)
{
	m_StringField1 = std::move(i_StringField1);
}

string Request::GetStringField2() const
{
	return m_StringField2;
}

void Request::SetStringField2(const string& i_StringField2)
{
	m_StringField2 = i_StringField2;
}

void Request::SetStringField2(string&& i_StringField2)
{
	m_StringField2 = std::move(i_StringField2);
}

time_t Request::GetDateField() const
{
	return m_DateField;
}

void Request::SetDateField(time_t i_DateField)
{
	m_DateField = i_DateField;
}

ReplyMessage::ReplyMessage(): m_StringField()
{
	SetType(eRequestType::Logout);
	SetReply(eReply::InsufficientPriviledge);
	SetIntField(0);
	SetBoolField1(false);
	SetBoolField2(false);
	SetDateField(NULL_DATE);
}

eRequestType ReplyMessage::GetType() const
{
	return m_Type;
}

void ReplyMessage::SetType(eRequestType i_Type)
{
	m_Type = i_Type;
}

eReply ReplyMessage::GetReply() const
{
	return m_Reply;
}

void ReplyMessage::SetReply(eReply i_Reply)
{
	m_Reply = i_Reply;
}

int ReplyMessage::GetIntField() const
{
	return m_IntField;
}

void ReplyMessage::SetIntField(int i_IntField)
{
	m_IntField = i_IntField;
}

string ReplyMessage::GetStringField() const
{
	return m_StringField;
}

void ReplyMessage::SetStringField(const string& i_StringField)
{
	m_StringField = i_StringField;
}

void ReplyMessage::SetStringField(string&& i_StringField)
{
	m_StringField = std::move(i_StringField);
}

bool ReplyMessage::GetBoolField1() const
{
	return m_BoolField1;
}

void ReplyMessage::SetBoolField1(bool i_BoolField1)
{
	m_BoolField1 = i_BoolField1;
}

bool ReplyMessage::GetBoolField2() const
{
	return m_BoolField2;
}

void ReplyMessage::SetBoolField2(bool i_BoolField2)
{
	m_BoolField2 = i_BoolField2;
}

time_t ReplyMessage::GetDateField() const
{
	return m_DateField;
}

void ReplyMessage::SetDateField(time_t i_DateField)
{
	m_DateField = i_DateField;
}

bool LazyParkingWindowsCPPClientBL::eReplyFromString(const string& i_String, eReply& o_EReply)
{
	static std::unordered_map<std::string, eReply> map;
	static bool isInitialized = false;

	if (!isInitialized)
	{
		map["Success"] = eReply::Success;
		map["SUCCESS"] = eReply::Success;
		map["UsernameNotFound"] = eReply::UsernameNotFound;
		map["USERNAME_NOT_FOUND"] = eReply::UsernameNotFound;
		map["WrongPW"] = eReply::WrongPW;
		map["WRONG_PW"] = eReply::WrongPW;
		map["InsufficientPriviledge"] = eReply::InsufficientPriviledge;
		map["INSUFFICIENT_PRIVILEDGE"] = eReply::InsufficientPriviledge;
		map["DateExpired"] = eReply::DateExpired;
		map["DATE_EXPIRED"] = eReply::DateExpired;
		map["IDNotFound"] = eReply::IDNotFound;
		map["ID_NOT_FOUND"] = eReply::IDNotFound;
		isInitialized = true;
	}

	auto iterator = map.find(i_String);
	if (iterator != map.end())
	{
		o_EReply = iterator->second;
		
		return true;
	}

	return false;
}

bool LazyParkingWindowsCPPClientBL::eRequestTypeFromString(const string& i_String, eRequestType& o_ERequestType)
{
	static std::unordered_map<std::string, eRequestType> map;
	static bool isInitialized = false;

	if (!isInitialized)
	{
		map["Login"] = eRequestType::Login;
		map["LOGIN"] = eRequestType::Login;
		map["Logout"] = eRequestType::Logout;
		map["LOGOUT"] = eRequestType::Logout;
		map["OpenGate"] = eRequestType::OpenGate;
		map["OPEN_GATE"] = eRequestType::OpenGate;
		map["GetParkingStatus"] = eRequestType::GetParkingStatus;
		map["GET_PARKING_STATUS"] = eRequestType::GetParkingStatus;
		map["ChangePW"] = eRequestType::ChangePW;
		map["CHANGE_PW"] = eRequestType::ChangePW;
		map["ReserveParkingSpot"] = eRequestType::ReserveParkingSpot;
		map["RESERVE_PARKING_SPOT"] = eRequestType::ReserveParkingSpot;
		map["AddDriver"] = eRequestType::AddDriver;
		map["ADD_DRIVER"] = eRequestType::AddDriver;
		map["RemoveDriver"] = eRequestType::RemoveDriver;
		map["REMOVE_DRIVER"] = eRequestType::RemoveDriver;
		map["CancelReservation"] = eRequestType::CancelReservation;
		map["CANCEL_RESERVATION"] = eRequestType::CancelReservation;
		map["ChangeExpiration"] = eRequestType::ChangeExpiration;
		map["CHANGE_EXPIRATION"] = eRequestType::ChangeExpiration;
		map["GetNextExpired"] = eRequestType::GetNextExpired;
		map["GET_NEXT_EXPIRED"] = eRequestType::GetNextExpired;
		isInitialized = true;
	}

	auto iterator = map.find(i_String);
	if (iterator != map.end())
	{
		o_ERequestType = iterator->second;

		return true;
	}

	return false;
}

bool LazyParkingWindowsCPPClientBL::stringFromEReply(const eReply& i_EReply, string& o_String)
{
	static std::unordered_map<int, std::string> map;
	static bool isInitialized = false;

	if (!isInitialized)
	{
		map[(int)eReply::Success] = "SUCCESS";
		map[(int)eReply::UsernameNotFound] = "USERNAME_NOT_FOUND";
		map[(int)eReply::WrongPW] = "WRONG_PW";
		map[(int)eReply::InsufficientPriviledge] = "INSUFFICIENT_PRIVILEDGE";
		map[(int)eReply::DateExpired] = "DATE_EXPIRED";
		map[(int)eReply::IDNotFound] = "ID_NOT_FOUND";
		isInitialized = true;
	}

	auto iterator = map.find((int)i_EReply);
	if (iterator != map.end())
	{
		o_String = iterator->second;

		return true;
	}

	return false;
}

bool LazyParkingWindowsCPPClientBL::stringFromERequestType(const eRequestType& i_ERequestType, string& o_String)
{
	static std::unordered_map<int, std::string> map;
	static bool isInitialized = false;

	if (!isInitialized)
	{
		map[(int)eRequestType::Login] = "LOGIN";
		map[(int)eRequestType::Logout] = "LOGOUT";
		map[(int)eRequestType::OpenGate] = "OPEN_GATE";
		map[(int)eRequestType::GetParkingStatus] = "GET_PARKING_STATUS";
		map[(int)eRequestType::ChangePW] = "CHANGE_PW";
		map[(int)eRequestType::ReserveParkingSpot] = "RESERVE_PARKING_SPOT";
		map[(int)eRequestType::AddDriver] = "ADD_DRIVER";
		map[(int)eRequestType::RemoveDriver] = "REMOVE_DRIVER";
		map[(int)eRequestType::CancelReservation] = "CANCEL_RESERVATION";
		map[(int)eRequestType::ChangeExpiration] = "CHANGE_EXPIRATION";
		map[(int)eRequestType::GetNextExpired] = "GET_NEXT_EXPIRED";
		isInitialized = true;
	}

	auto iterator = map.find((int)i_ERequestType);
	if (iterator != map.end())
	{
		o_String = iterator->second;

		return true;
	}

	return false;
}