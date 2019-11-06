#include<string>
#include<vector>

#include "../cqp/cqp.h"
#include "curlpost.h"
#include "./MJson.h"

#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载

using namespace std;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;
vector<int64_t> uid;
vector<char *> saveInput;


/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/* 用于转义的消除与增加 */
string static strToggle(const string & input)
{
	string output("");
	for (size_t i = 0; i < input.length();)
	{
		if (i + 4 < input.length())
		{
			string sub = input.substr(i, 5);
			if (sub == "&amp;")
			{
				output.append("&");
				i += 5;
				continue;
			}
			else if (sub == "&#91;") {
				output.append("[");
				i += 5;
				continue;
			}
			else if (sub == "&#93;") {
				output.append("]");
				i += 5;
				continue;
			}
			else if (sub == "&#44;") {
				output.append(",");
				i += 5;
				continue;
			}
		}
		if (input[i] == '&') {
			output.append("&amp;");
			++i;
		}
		else if (input[i] == '[') {
			output.append("&#91;");
			++i;
		}
		else if (input[i] == ']') {
			output.append("&#93;");
			++i;
		}
		else if (input[i] == ']') {
			output.append("&#44;");
			++i;
		}
		else {
			output+=input[i];
			++i;
		}
	}
	return output;
}
int Between(char* pcBuf, char* pcRes)
{
	char* pcBegin = NULL;
	char* pcEnd = NULL;

	pcBegin = strstr(pcBuf, "input");
	pcEnd = strstr(pcBuf, "inputend");

	if (pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
	{
		printf("Mail name not found!\n");
	}
	else
	{
		pcBegin += strlen(":");
		memcpy(pcRes, pcBegin, pcEnd - pcBegin);
	}

	return 0;
}

/* 用于参数的获取 */
static vector<string> getarg(const string & str) {
	int flag = 0;
	vector<string> vec(3);
	for (size_t i = 0; i < str.length(); ++i) {
		if (isspace(static_cast<unsigned char>(str[i])) || isblank(static_cast<unsigned char>(str[i]))) {
			if (flag == 0) {
				if (vec[0] == "")continue;
				++flag; continue;
			}
			else if (flag == 1) {
				if (vec[1] == "")continue;
				++flag; continue;
			}
			else {
				if (vec[2] == "")continue;
			}
		}
		vec[flag] += str[i];
	}
	return vec;
}

string runall(const char* lan, const char* content,const char* input) noexcept
{
	//MessageBoxA(NULL, content, "", 0);
	static vector<pair<string, string>> lanmap
	{
		{"assembly","main.asm"},
		{"ats","main.dats"},
		{"bash","main.sh"},
		{"c","main.c"},
		{"clojure","main.clj"},
		{"cobol","main.cob"},
		{"coffeescript","main.coffee"},
		{"cpp","main.cpp"},
		{"crystal","main.cr"},
		{"csharp","main.cs"},
		{"d","main.d"},
		{"elixir","main.ex"},
		{"elm","Main.elm"},
		{"erlang","main.erl"},
		{"fsharp","main.fs"},
		{"go","main.go"},
		{"groovy","main.groovy"},
		{"haskell","main.hs"},
		{"idris","main.idr"},
		{"java","Main.java"},
		{"javascript","main.js"},
		{"julia","main.jl"},
		{"kotlin","main.kt"},
		{"lua","main.lua"},
		{"mercury","main.m"},
		{"nim","main.nim"},
		{"ocaml","main.ml"},
		{"perl","main.pl"},
		{"perl6","main.pl6"},
		{"php","main.php"},
		{"plaintext","main.txt"},
		{"python","main.py"},
		{"ruby","main.rb"},
		{"rust","main.rs"},
		{"scala","main.scala"},
		{"swift","main.swift"},
		{"typescript","main.ts"}
	};
	auto split = [](string src, const string& substr) ->vector<string> {
		vector<string> retvec;
		size_t pos;
		while (true)
		{
			if ((pos = src.find_first_of(substr)) == string::npos) {
				retvec.push_back(src);
				break;
			}
			else {
				retvec.push_back(src.substr(0, pos));
				src = src.substr(pos + 1);
			}
		}
		return retvec;
	};
	for (const pair<string, string>& i : lanmap) {
		string filename = i.second;
		string website = "https://glot.io/run/" + i.first + "?version=latest";
		string ret;
		//split(filename, ".").at(1)
		if (split(filename, ".").at(1) == lan) {
			try {
				Json::Value  J;
				J["files"][0]["name"] = filename;
				J["files"][0]["content"] = Http::GBKToUTF8(content);
				if (input != NULL) {
					J["stdin"] = strToggle(input);
				}
				
				Json::FastWriter fastWriter;
				string postdata = fastWriter.write(J);
				ret = Http(website, { { "User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36" } }).Post(postdata);
				return Http::UTF8ToGBK(MJson(ret)["stdout"].asString() + MJson(ret)["error"].asString() + MJson(ret)["stderr"].asString());
			} 
			catch (std::exception& e) {
				return   Http::UTF8ToGBK(ret)+ e.what();
			}
		}
	}
	for (const pair<string, string>& i : lanmap) {
		if (lan == i.first) {
			return "Please reply:\ncode " + split(i.second, ".").at(1) + "\nYourCode";
		}
	}
	return "No this language";
}

char* GetInput(char* msg) {
	char* m = strstr(msg, "input");
	if (m != NULL) {
		int length = strlen(msg);
		char* a = new char[length - 4];
		memcpy(a, msg + 5, length - 4);
		return a;
	}
	return NULL;
}
void Add(int64_t qq, char* msg) {
	//CQ_sendGroupMsg(ac, 750462328, msg); 
	for (int i = 0; i < uid.size(); i++) {
		if (uid[i] == qq) {
			saveInput[i] = msg;
			return;
		}
	}
	uid.push_back(qq);
	saveInput.push_back(msg);
}

char* GetSaveInput(int64_t qq) {
	for (int i = 0; i < uid.size(); i++) {
		if (uid[i] == qq) {
			return saveInput[i];
		}
	}
	return NULL;
}
/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	vector<string> vec = getarg(msg);
	//获取input
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("已保存输入\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "已保存输入\n");

		strcat(reply, input);
		CQ_sendPrivateMsg(ac, fromQQ, reply);
	}


	if (vec[0] == "code")
	{
		//消除转义
		vec[2] = strToggle(vec[2]);

		//获取运行结果
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//增加转义
		ret = strToggle(ret);

		//增加AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "回应过长，已做截断处理\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendPrivateMsg(ac, fromQQ, ret.c_str());
	}
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	vector<string> vec = getarg(msg);
	//获取input
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("已保存输入\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "已保存输入\n");
		strcat(reply, input);
		CQ_sendGroupMsg(ac, fromGroup, reply);
	}
	
	
	if (vec[0] == "code")
	{
		//消除转义
		vec[2] = strToggle(vec[2]);

		//获取运行结果
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//增加转义
		ret = strToggle(ret);

		//增加AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "回应过长，已做截断处理\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendGroupMsg(ac, fromGroup, ret.c_str());
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	vector<string> vec = getarg(msg);
	//获取input
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("已保存输入\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "已保存输入\n");
		strcat(reply, input);
		CQ_sendDiscussMsg(ac, fromDiscuss, reply);
	}


	if (vec[0] == "code")
	{
		//消除转义
		vec[2] = strToggle(vec[2]);

		//获取运行结果
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//增加转义
		ret = strToggle(ret);

		//增加AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "回应过长，已做截断处理\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendDiscussMsg(ac, fromDiscuss, ret.c_str());
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

///*
//* 菜单，可在 .json 文件中设置菜单数目、函数名
//* 如果不使用菜单，请在 .json 及此处删除无用菜单
//*/
//CQEVENT(int32_t, __menuA, 0)() {
//	//MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
//	return 0;
//}
//
//CQEVENT(int32_t, __menuB, 0)() {
//	//MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
//	return 0;
//}
