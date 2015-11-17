#include "ServiceListener.h"

ServiceListener::ServiceListener(const http::uri& url) : m_listener(http_listener(url))

{

	m_listener.support(methods::POST, std::tr1::bind(&ServiceListener::handle_post,this,std::tr1::placeholders::_1));

	m_listener.support(methods::OPTIONS,std::tr1::bind(&ServiceListener::handle_options, this, std::tr1::placeholders::_1));
	
	try
	{
		m_listener.open().wait();
		cout << "Service is listening for requests..." << endl;
	}
	catch (...) {
		cout << "Cannot listen on port 8080! Make sure another instance is not running." << endl;
	}
}

void ServiceListener::handle_options(http_request request){

	http_response response(status_codes::OK);
	response.headers().add(L"Access-Control-Allow-Origin", L"*");
	response.headers().add(L"Access-Control-Allow-Headers", L"Origin, X-Requested-With, Content-Type, Accept");

	request.reply(response);
}

void ServiceListener::handle_post(http_request message)
{

	message.extract_string().then([=](utility::string_t s1){
		error_code ec;
		http_response response(status_codes::BadRequest);
		response.headers().add(L"Access-Control-Allow-Origin", L"*");
		response.headers().add(L"Access-Control-Allow-Headers", L"Origin, X-Requested-With, Content-Type, Accept");
		json::value v1 = json::value::parse(s1, ec);
		if (!v1.has_field(L"data")) {
			cout << "doesnt have field \"data\"\n";
		}
		else {
			std::wstring data = v1[L"data"].as_string();
			str = wStringToString(data);

		}
		  
		string decoded = urlDecoder(str);
			
		response.set_status_code(status_codes::OK);
		response.set_body(getFinalData(decoded));
		return message.reply(response);
	});
};

string ServiceListener::wStringToString(std::wstring source)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(source);
}

u32string ServiceListener::To_UTF32(const string &s)
{
	wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(s);
}

string ServiceListener::To_UTF8(const u32string &s)
{
	wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
}

string ServiceListener::urlDecoder(string& SRC)
{
	string ret;
	char ch; 
	int i, ii;
	for (i = 0; i < SRC.length(); i++)
	{
		if (int(SRC[i]) == 37)
		{
			sscanf_s(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
		else
		{
			ret += SRC[i];
		}
	}
	return(ret);
}


string ServiceListener::getFinalData(string data)
{
	ios_base::sync_with_stdio(false);
	try {
		Circuit circuit;
		out.open("VerilogData.txt");
		out << data;
		out.close();
		circuit.parseFile("VerilogData.txt");
		Wavedrom encoder(circuit);
	
		string jsonResult = encoder.getJSON();

		int h = jsonResult.find("assign");
		if (h != -1) {
			jsonResult.replace(h, 6, "\"assign\"");
		}

		for (int i = 0; i < jsonResult.length(); i++) {
			if (jsonResult[i] == '\'') {
				jsonResult[i] = '"';
			}
		}

		out.open("VerilogData.txt");
		out << jsonResult;
		out.close();

		return jsonResult;
	}
	catch (ParseError& e) {
		cout << "ParseError: " << e.what() << endl;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

}