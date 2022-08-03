#pragma once
#include <ctime>
#include <string>

#define NULL_DATE 0

using std::string;

namespace LazyParkingWindowsCPPClientBL
{
	enum class eRequestType
	{
		Login,
		Logout,
		OpenGate,
		GetParkingStatus,
		ChangePW,
		ReserveParkingSpot,
		AddDriver,
		RemoveDriver,
		CancelReservation,
		ChangeExpiration,
		GetNextExpired
	};

	class Request
	{
		eRequestType m_Type;
		int m_IntField;
		string m_StringField1;
		string m_StringField2;
		time_t m_DateField;

	public:
		Request();
		eRequestType GetType() const;
		void SetType(eRequestType i_Type);
		int GetIntField() const;
		void SetIntField(int i_IntField);
		string GetStringField1() const;
		void SetStringField1(const string& i_StringField1);
		void SetStringField1(string&& i_StringField1);
		string GetStringField2() const;
		void SetStringField2(const string& i_StringField2);
		void SetStringField2(string&& i_StringField2);
		time_t GetDateField() const;
		void SetDateField(time_t i_DateField);
	};

	enum class eReply {
		Success,
		UsernameNotFound,
		WrongPW,
		InsufficientPriviledge,
		DateExpired,
		IDNotFound,
	};

	class ReplyMessage
	{
		eRequestType m_Type;
		eReply m_Reply;
		int m_IntField;
		string m_StringField;
		bool m_BoolField1;
		bool m_BoolField2;
		time_t m_DateField;

	public:
		ReplyMessage();
		eRequestType GetType() const;
		void SetType(eRequestType i_Type);
		eReply GetReply() const;
		void SetReply(eReply i_Reply);
		int GetIntField() const;
		void SetIntField(int i_IntField);
		string GetStringField() const;
		void SetStringField(const string& i_StringField);
		void SetStringField(string&& i_StringField);
		bool GetBoolField1() const;
		void SetBoolField1(bool i_BoolField1);
		bool GetBoolField2() const;
		void SetBoolField2(bool i_BoolField2);
		time_t GetDateField() const;
		void SetDateField(time_t i_DateField);
	};

	bool eReplyFromString(const string& i_String, eReply& o_EReply);
	
	bool stringFromEReply(const eReply& i_EReply, string& o_String);
	
	bool eRequestTypeFromString(const string& i_String, eRequestType& o_ERequestType);
	
	bool stringFromERequestType(const eRequestType& i_ERequestType, string& o_String);
}