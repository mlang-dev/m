struct BaseStruct{
	int a;
	int b;
	int c;
	int d;
	int e;
};

struct BaseStruct get_struct()
{
	struct BaseStruct bs;
	bs.a = 100;
	bs.b = 200;
	bs.c = 300;
	bs.d = 400;
	bs.e = 500;
	return bs;
}
