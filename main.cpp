#include <cstdlib>
#include <fstream>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <message.capnp.h>

std::string generate_name()
{
	char buf[17] = {};

	size_t len = ((unsigned int)std::rand() & 15) + 1;

	buf[0] = (char)((std::rand() % 25) + 'A');
	for (size_t i = 1, end = (size_t)len; i < end; ++i)
	{
		buf[i] = (char)((std::rand() % 25) + 'a');
	}

	return buf;
}

std::string read(const uint8_t *data, size_t data_len)
{
	// Frustrationslevel: Maximum.
	// Auffindbarkeit der zu verwendenden Datentypen gleich Null.
	// Verstehbarkeit der zu verwendenden Datentypen gleich Null. Keine Beispiele vorhanden.
	// minimal langsamer als flatbuffers (+2ms). Wahrscheinlich wegen anlegen von NullArrayDisposer, Array und ArrayInputStream.

	// Option 1: specify file descriptor
	//capnp::StreamFdMessageReader message(in_fd);
	// Option 2: Use ArrayInputStream
	kj::NullArrayDisposer disposer;
	auto arr = kj::Array<const uint8_t>(data, data_len, disposer);
	kj::ArrayInputStream instream(arr);
	capnp::InputStreamMessageReader message(instream);
	// Ende Option 2

	Addressbook::Reader addressbook = message.getRoot<Addressbook>();
	capnp::List<Person>::Reader people = addressbook.getPeople();

	std::string retval;
	retval.reserve(64);
	retval += people.begin()->getName().cStr();
	retval += " ";
	retval += (people.end() - 1)->getName().cStr();

	return retval;
}

void write(int out_fd)
{
	capnp::MallocMessageBuilder message;
	Addressbook::Builder addressbookbuilder = message.initRoot<Addressbook>();
	::capnp::List<Person>::Builder people = addressbookbuilder.initPeople(1000);

	for (size_t i = 0; i < 1000; ++i)
	{
		people[i].setId(i);
		people[i].setName(generate_name());
	}

	capnp::writeMessageToFd(out_fd, message);
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int main()
{
	LARGE_INTEGER freq, start, stop;

	std::srand(222);
	FILE *out = fopen("out.bin", "wb");
	int out_fd = ::fileno(out);
	write(out_fd);
	fclose(out);

	std::ifstream in;
	in.open("out.bin", std::ios::binary | std::ios::in);
	in.seekg(0, std::ios::end);
	size_t data_len = in.tellg();
	in.seekg(0, std::ios::beg);
	uint8_t *data = new uint8_t[data_len];
	in.read((char*)data, data_len);
	in.close();

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	for (size_t i = 0; i < 1000; ++i)
	{
		if (read(data, data_len)[0] == 'a') { // just so the compiler won't optimize away
			std::cout << 'a' << std::endl;
		}
	}

	QueryPerformanceCounter(&stop);
	stop.QuadPart -= start.QuadPart;

	std::cout << "Execution took " << ((stop.QuadPart * 1000.f) / freq.QuadPart) << "ms";

	std::cin.ignore(1024, '\n');
	delete[] data;

	return 0;
}