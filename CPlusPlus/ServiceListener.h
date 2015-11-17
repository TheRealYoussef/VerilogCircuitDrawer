#pragma once
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include "Circuit.h"
#include "ParseError.h"
#include "Wavedrom.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web::json;
using namespace web;
using namespace std;
using namespace utility;
using namespace utility::conversions;

class ServiceListener
{
public:
	ServiceListener(const http::uri& url);
private:
	void handle_post(http_request request);
	void handle_options(http_request request);
	ifstream in;
	ofstream out;
	string readFromFile;
	http_listener m_listener;
	static u32string To_UTF32(const string &s);
	static string To_UTF8(const u32string &s);
	string wStringToString(std::wstring source);
	string urlDecoder(string& SRC);
	string getFinalData(string data);
	string str;
};