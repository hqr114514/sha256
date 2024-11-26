#include "sha256.h"

int main(int argc, char** argv) {
	if (argc < 2) return 1;
	char* FileName = argv[1];
	uc out[32] = { 0 };
	sha256(FileName, out);
	for (int i = 0; i < 32; i++) printf("%02x", out[i]);
	cout << endl;
	return 0;
}