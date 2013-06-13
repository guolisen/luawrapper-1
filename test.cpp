#include "luaWrapper.h"
#include <stdio.h>

typedef lWrapper::luatable luatable;
typedef lWrapper::luaObject luaObject;
#define any_cast lWrapper::any_cast

//测试直接调用C函数
static int showmsg(const char *msg)
{
	printf("c-showmsg:%s\n",msg);
	return 0;
}

void test_call_c_function(lua_State *L)
{
	printf("-------测试调用C函数---------\n");
	lWrapper::register_function(L,"cshow",showmsg);
	try{
		lWrapper::call<void>("test1",L);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

class test_class_A
{
	public:
		int  memb_a;
		void show()
		{
			printf("test_class_A::show\n");
		}
};

void test_class1(lua_State *L)
{
	printf("\n-----测试向lua传递c++对象1-----\n");
	lWrapper::register_class<test_class_A>(L,"test_class_A")
		.property("memb_a",&test_class_A::memb_a)
		.function("show",&test_class_A::show);
	
	test_class_A a;
	a.memb_a = 100;
	try{
		lWrapper::call<void>("test2",L,&a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("调用完test2之后,a.memb_a:%d\n",a.memb_a);
}

void test_pass_luatable(lua_State *L)
{
	printf("\n-----测试向lua传递luatable-----\n");
	try{
		luatable lt;
		for(int i = 0; i < 10; ++i)
			lt.push_back(i);
		lWrapper::call<void>("test3",L,lt);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

luatable c_return_luatable()
{
	luatable lt;
	for(int i = 0; i < 10; ++i)
		lt.push_back(i);
	return lt;
}

void test_c_return_luatable(lua_State *L)
{
	printf("\n-----测试向lua返回luatable-----\n");
	lWrapper::register_function(L,"c_return_luatable",c_return_luatable);
	try{
		lWrapper::call<void>("test4",L);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

void test_lua_return_luatable(lua_State *L)
{
	printf("\n-----测试从lua返回luatable-----\n");
	try{
		luatable lt = lWrapper::call<luatable>("test5",L);
		for(int i = 0 ;i < lt.size(); ++i)
			printf("%d\n",any_cast<int>(lt[i]));
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_op_lua_obj(lua_State *L)
{
	printf("\n-----测试操作lua对象-----\n");
	try{
		luaObject lo = lWrapper::call<luaObject>("test6",L);
		lo.call<void>("show");
		printf("balance:%d\n",lo.Get<int>("balance"));
		lo.Set("balance",1000);
		printf("balance:%d\n",lo.Get<int>("balance"));		
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

class base
{
	public:
		virtual void show()
		{
			printf("this base show\n");
		}
		
		void show2()
		{
			printf("this is not virtual function\n");
		}
};

class child:public base
{
public:
	void show()
	{
		printf("this child show\n");
	}
};

void test_call_virtual_function(lua_State *L)
{
	printf("\n-----测试lua调用c++对象虚函数-----\n");
	lWrapper::register_class<base>(L,"base")
		.function("show",&base::show)
		.function("show2",&base::show2);
	lWrapper::register_class<child,base>(L,"child");
	child c;
	base *b = &c;
	try{
		lWrapper::call<void>("test7",L,b,&c);	
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_int64(lua_State *L)
{
	printf("\n-----测试lua中使用64位整数-----\n");
	try{
		int64_t a = 4294967295;
		int64_t b = 4294967296;
		lWrapper::call<void>("test8",L,a,b);	
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

int main()
{
	lWrapper::luaWrapper lw;
	lw.dofile("start.lua");
	test_call_c_function(lw);
	test_class1(lw);
	test_pass_luatable(lw);
	test_c_return_luatable(lw);
	test_lua_return_luatable(lw);
	test_op_lua_obj(lw);
	test_call_virtual_function(lw);
	test_int64(lw);
	getchar();
	return 0;
}