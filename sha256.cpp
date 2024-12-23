#include "sha256.h"

#define S(x,y) (((x)>>(y))|(x)<<(32-(y)))
#define swap32(x) ((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24))
#define Ch(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define Ma(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
#define S0(x) (S((x),2)^S((x),13)^S((x),22))
#define S1(x) (S((x),6)^S((x),11)^S((x),25))
#define o0(x) (S((x),7)^S((x),18)^((x)>>3))
#define o1(x) (S((x),17)^S((x),19)^((x)>>10))

const ui k[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

ui H[8] = {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19
};

void block(uc data[64]) {
	ui w[64] = { 0 };//将512bit转换为16个32bit的无符号整数
	//转换:
	for (int t = 0, j = 0; t < 16; t++, j += 4) w[t] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | data[j + 3];
	//扩充:
	for (int t = 16; t < 64; t++) w[t] = o1(w[t - 2]) + w[t - 7] + o0(w[t - 15]) + w[t - 16];
	ui a = H[0];
	ui b = H[1];
	ui c = H[2];
	ui d = H[3];
	ui e = H[4];
	ui f = H[5];
	ui g = H[6];
	ui h = H[7];
	for (int i = 0; i < 64; i++) {
		ui T1 = h + S1(e) + Ch(e, f, g) + k[i] + w[i];
		ui T2 = S0(a) + Ma(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
	}
	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
	H[5] += f;
	H[6] += g;
	H[7] += h;
	return;
}

void sha256(const char* FileName,uc* out) {
	ifstream fin(FileName, ios::binary);
	fin.seekg(0, ios::end);//文件指针移动至最后
	ull siz = fin.tellg();//获取总长度
	fin.seekg(0, ios::beg);//文件指针移到开头
	uc data[64] = { 0 };//64B = 512bit Data
	while (fin.read((char*)data, 64)) {//循环处理512bit的块
		//处理块:
		block(data);
		memset(data, 0, 64 * sizeof(uc));
	}
	//处理剩余
	ui rest = fin.gcount();
	data[rest] = 0x80;//填1
	for (int i = rest + 1; i < 64; i++) data[i] = 0;//填0
	if (rest > 55) {
		//要分两块
		block(data);
		memset(data, 0, 64 * sizeof(uc));
	}
	for (int i = 56, j = 0; i < 64; i++, j++) {//8byte(64bit) length information
		data[i] = (siz * 8) >> ((7 - j) * 8);
	}
	block(data);
	fin.close();
	for (int i = 0, j = 0; i < 32; i++, j += 8) {
		if (j == 32) j = 0;
		out[i] = swap32(H[i / 4]) >> j;
	}
	return;
}
