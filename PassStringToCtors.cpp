#include <iostream>
#include <format>

class DbgStr
{
	std::size_t _size;
	std::unique_ptr<char[]> _s;

	static inline int _cStrCtor = 0;
	static inline int _defaultCtor = 0;
	static inline int _copyCtor = 0;
	static inline int _copyAssign = 0;
	static inline int _moveCtor = 0;
	static inline int _moveAssign = 0;

public:
	static void resetCounters()
	{
		_cStrCtor = 0;
		_defaultCtor = 0;
		_copyCtor = 0;
		_copyAssign = 0;
		_moveCtor = 0;
		_moveAssign = 0;
	}

	static void printAndResetStats()
	{
		printStats();
		resetCounters();
	}

	void print()
	{
		std::cout << _s.get() << std::endl;
	}

	void static printStatsHeader()
	{
		std::cout << "ctor(cstr)\t";
		std::cout << "ctor()\t";
		std::cout << "copy()\t";
		std::cout << "copy=\t";
		std::cout << "move()\t";
		std::cout << "move=\t";
		std::cout << std::endl;
	}

	void static printStats()
	{
		std::cout << std::format("{}\t\t", _cStrCtor);
		std::cout << std::format("{}\t", _defaultCtor);
		std::cout << std::format("{}\t", _copyCtor);
		std::cout << std::format("{}\t", _copyAssign);
		std::cout << std::format("{}\t", _moveCtor);
		std::cout << std::format("{}\t", _moveAssign);
	}

	DbgStr(const char* cstring) :
		_size(strlen(cstring)+1),
		_s(std::make_unique<char[]>(_size))
	{
		++_cStrCtor;
		strcpy_s(_s.get(), _size, cstring);
	}

	DbgStr() : _size(0)
	{
		++_defaultCtor;
	}

	DbgStr(const DbgStr& other) :
		_size(other._size),
		_s(std::make_unique<char[]>(_size))
	{
		++_copyCtor;
		strcpy_s(_s.get(), _size, other._s.get());
	}

	DbgStr& operator=(const DbgStr& other)
	{
		++_copyAssign;
		if (this == &other)
			return *this;
		return *this;
	}

	DbgStr(DbgStr&& other) noexcept :
		_size(other._size),
		_s(std::move(other._s))
	{
		++_moveCtor;
		other._size = 0;
	}

	DbgStr& operator=(DbgStr&& other) noexcept
	{
		++_moveAssign;
		if (this == &other)
			return *this;
		return *this;
	}
};

struct UserConstRef
{
	DbgStr name;

	UserConstRef(const DbgStr& str) : name(str) { }
};

struct UserByVal
{
	DbgStr name;

	UserByVal(DbgStr s) : name(std::move(s)) { }
};

struct UserByRVal
{
	DbgStr name;

	UserByRVal(DbgStr&& s) : name(s) { }
};

template <typename T>
concept StrignConvertable = 
	std::is_convertible_v<T, DbgStr>;

struct UserPerfectFwd
{
	DbgStr name;

	template<StrignConvertable T>
	UserPerfectFwd(T&& str) : name(std::forward<T>(str)) { }
};

template<typename TUser>
void Test()
{
	{
		TUser user { "test string" };
		DbgStr::printAndResetStats();
		std::cout << std::format("{} from a string literal\t", typeid(TUser).name());
		user.name.print();
	}

	{
		if constexpr (std::is_constructible_v<TUser, DbgStr&>)
		{
			DbgStr str { "test string" };
			TUser user{ str };
			DbgStr::printAndResetStats();
			std::cout << std::format("{} from l-value reference\t", typeid(TUser).name());
			user.name.print();
		}
	}

	{
		DbgStr str{ "test string" };
		TUser user{ std::move(str) };
		DbgStr::printAndResetStats();
		std::cout << std::format("{} from r-value reference\t", typeid(TUser).name());
		user.name.print();
	}

	{
		TUser user{ DbgStr { "test string" } };
		DbgStr::printAndResetStats();
		std::cout << std::format("{} from r-value reference2\t", typeid(TUser).name());
		user.name.print();
	}
	std::cout << std::endl;
}

int main()
{
	DbgStr::printStatsHeader();

	Test<UserConstRef>();
	Test<UserByVal>();
	Test<UserByRVal>();
	Test<UserPerfectFwd>();

	std::cout << "done";
}


