#include <stdio.h>
#include"Huffman.h"
#include"Encoding.h"
#include"Decoding.h"

#define  MAX_NODE  256*9
#define MAX_BIT_LEN  10000
int soNodeTrongCay = 0;

struct NODE {
	unsigned char   c;      // ky tu
	int     freq;			// so lan xuat hien
	bool    used;			// danh dau node da xu ly chua
	int     nLeft;			// chi so nut con nam ben trai
	int     nRight;			// chi so nut con nam ben phai
};

struct MABIT {
	char*   bits;
	int     soBit;
};

NODE    huffTree[MAX_NODE];
MABIT   bangMaBit[256];

void KhoiTao() {
	for (int i = 0; i < MAX_NODE; i++) {
		huffTree[i].c = i;
		huffTree[i].freq = 0;
		huffTree[i].used = false;
		huffTree[i].nLeft = -1;
		huffTree[i].nRight = -1;
	}
}



void ThongKeTanSoXuatHien(char* tenFile) {
	FILE* fi = fopen(tenFile, "rb");
	unsigned char c;

	while (1) {
		fread(&c, 1, sizeof(char), fi);
		if (feof(fi)) {
			break;
		}
		huffTree[c].freq++; 
	}

	fclose(fi);
}

void XuatBangThongKe() {
	printf("Bang thong ke tan suat: \n");
	for (int i = 0; i < 256; i++) {
		if (huffTree[i].freq > 0)
		{
			printf("%c: %d\n", i, huffTree[i].freq);
			soNodeTrongCay++;
		}
	}
}

bool Tim2PhanTuMin(int &i, int &j, int nNode) {
	i = -1;
	j = -1;

	// tim 2 phan tu co trong so nho nhat
	for (int k = 0; k < nNode; k++)
		if (huffTree[k].used == false && huffTree[k].freq > 0) { 
			if (i == -1) {
				i = k;
			}
			else if (j == -1) {
				j = k;
			}
			else {
				if (huffTree[i].freq > huffTree[j].freq) {
					if (huffTree[k].freq < huffTree[i].freq) {
						i = k;
					}
				}
				else {
					if (huffTree[k].freq < huffTree[j].freq) {
						j = k;
					}
				}
			}
		}
	// sap xep lai 2 phan tu de co i: phan tu co trong so nho nhat, j: phan tu co trong so nho nhi 
	// co 2 truong hop can doi i,j:
	// huffTree[i].freq > huffTree[j].freq
	// huffTree[i].freq == huffTree[j].freq va (huffTree[i].c > huffTree[j].c)
	if (i != -1 && j != -1) {
		if ((huffTree[i].freq > huffTree[j].freq) || ((huffTree[i].freq > huffTree[j].freq) && (huffTree[i].c > huffTree[j].c))) {
			int t = i;
			i = j;
			j = t;
		}
		return true;
	}
	else {
		return false;
	}
}

int TaoCayHuffman() {
	int nNode = 256;
	int i, j;
	bool timThay = false;
	while (true) {

		
		timThay = Tim2PhanTuMin(i, j, nNode);
		if (!timThay) {
			break;
		}

		
		huffTree[nNode].c = (huffTree[i].c < huffTree[j].c) ? huffTree[i].c : huffTree[j].c;
		huffTree[nNode].freq = huffTree[i].freq + huffTree[j].freq;
		huffTree[nNode].nLeft = i;
		huffTree[nNode].nRight = j;

		
		huffTree[i].used = true;
		huffTree[j].used = true;

		
		huffTree[nNode].used = false;
		nNode++;

	}
	return nNode - 1; 

}

void XuatCayHuffman(int node, int tab) {
	if (node == -1) {
		return;
	}
	for (int i = 0; i < tab; i++) {
		printf("\t");
	}
	if (huffTree[node].nLeft == -1 && huffTree[node].nRight == -1) {
		printf("%c\n", huffTree[node].c);
	}
	else {
		printf("%c..\n", huffTree[node].c);
		XuatCayHuffman(huffTree[node].nLeft, tab + 1);
		XuatCayHuffman(huffTree[node].nRight, tab + 1);
	}
}

void DuyetCayHuffman(int node, char maBit[], int nMaBit) {
	if (node == -1) {
		return;
	}
	if (huffTree[node].nLeft == -1 && huffTree[node].nRight == -1) {   
		bangMaBit[node].soBit = nMaBit;
		bangMaBit[node].bits = new char[nMaBit];
		for (int i = 0; i < nMaBit; i++) {
			bangMaBit[node].bits[i] = maBit[i];
		}
		return;
	}
	else {
		// Duyet qua ben trai thi gan bit 0
		maBit[nMaBit] = 0;
		DuyetCayHuffman(huffTree[node].nLeft, maBit, nMaBit + 1);

		// Duyet qua ben phai thi gan bit 1
		maBit[nMaBit] = 1;
		DuyetCayHuffman(huffTree[node].nRight, maBit, nMaBit + 1);
	}
}

void PhatSinhMaBit(int nRoot) { 
	for (int i = 0; i < 256; i++) {
		bangMaBit[i].soBit = 0;
		bangMaBit[i].bits = NULL;
	};
	char maBit[MAX_BIT_LEN / 8];
	int nMaBit = 0;

	DuyetCayHuffman(nRoot, maBit, nMaBit);
}

void XuatBangMaBit() {
	for (int i = 0; i < 256; i++)
		if (bangMaBit[i].soBit > 0) {
			printf("%c: ", i);
			for (int j = 0; j < bangMaBit[i].soBit; j++)
				printf("%d", bangMaBit[i].bits[j]);
			printf("\n");
		}
}

void XuatByte(unsigned char out, int soBitCoNghia) {
	for (int i = 7; i >= 7 - soBitCoNghia + 1; i--) {
		if ((out >> i) & 1) {
			printf("1");
		}
		else {
			printf("0");
		}
	}
	printf(" ");
}

void GhiBangThongKe(FILE *&fpwrite) {
	fwrite(&soNodeTrongCay, sizeof(int), 1, fpwrite);
	for (int i = 0; i < 256; i++)
	{
		if (huffTree[i].freq > 0)
		{
			char c = i;
			fwrite(&c, 1, sizeof(char), fpwrite);
			int n = huffTree[i].freq;
			fwrite(&n, 1, sizeof(int), fpwrite);
		}
	}


}


void MaHoa1KyTu(FILE *&fpwrite, unsigned char c, unsigned char &out, unsigned char &viTriBit) { 

	for (int i = 0; i < bangMaBit[c].soBit; i++) {
		if (bangMaBit[c].bits[i] == 1) {
			out = out | (1 << viTriBit); 
		}
		if (viTriBit == 0) { // 1 byte, ghi byte do ra file
			viTriBit = 7;

			fwrite(&out, 1, sizeof(char), fpwrite); //Ghi ma ra file ma nhi phan theo tung block 1 byte (8 bits)
			out = 0;
		}
		else {
			viTriBit--;
		}
	}
}


void NenHuffman(char* inputFile, char* outputFile) {

	FILE *fpwrite = fopen(outputFile, "wb");

	// BUOC 1: thong ke tan suat xuat hien cua cac ki tu
	KhoiTao();

	ThongKeTanSoXuatHien(inputFile);

	XuatBangThongKe();

	GhiBangThongKe(fpwrite); 

	 // BUOC 2: tao cay Huffman

	int nRoot = TaoCayHuffman();

	XuatCayHuffman(nRoot, 0);

	// BUOC 3: phat sinh ma bit

	PhatSinhMaBit(nRoot);

	XuatBangMaBit();

	// BUOC 4: thay the cac ky tu bang ma bit tuong ung
	unsigned char out = 0;              // ky tu se xuat ra
	unsigned char soBitCoNghia = 0;     // byte cuoi co the ko su dung het cac bit nen can luu so bit co nghia cua byte cuoi

	unsigned char c;
	unsigned char viTriBit = 7;         
	FILE* fi = fopen(inputFile, "rb");
	while (true) {
		fread(&c, 1, sizeof(char), fi);
		if (feof(fi)) {
			break;
		}
		MaHoa1KyTu(fpwrite, c, out, viTriBit);
		//Ghi ma ra file ma nhi phan theo tung block byte
	}
	soBitCoNghia = 7 - viTriBit;
	if (soBitCoNghia == 0) {
		soBitCoNghia = 8;
	}
	else {
		fwrite(&out, 1, sizeof(char), fpwrite); 
		//Ghi ma ra file ma nhi phan theo tung block byte
	}

	fclose(fi);
	fclose(fpwrite);
}

void LoadHuffmanTree(FILE *&fpread)
{
	//Reset lai cay Huffman va load lai
	KhoiTao(); 
	unsigned char c;
	unsigned int n;

	int soNode;
	fread(&soNode, sizeof(int), 1, fpread);

	for (int i = 0; i<soNode; i++)
	{
		fread(&c, sizeof(char), 1, fpread);
		fread(&n, sizeof(int), 1, fpread);
		huffTree[c].freq = n;
	}
}

int DemSoKiTu()
{
	int sum = 0;
	for (int i = 0; i < 256; i++) {
		if (huffTree[i].freq > 0) {
			sum += huffTree[i].freq;
		}
	}
	return sum;
}

int getbit(unsigned char n, int i)
{
	return (n >> i) & 0x1;
}

void onbit(char unsigned &n, int i) //Ham bat bit
{
	n = n | (0x1 << i);
}

void GiaiNen(int nRoot, FILE *& fpread, char* decompession)
{

	FILE *fdecode = fopen(decompession, "wb");

	int nCurrent = nRoot;
	//Dem so ki tu de ko bi du bit (vi doc theo block 8 bit)
	int soKiTu = DemSoKiTu(); 
	int isoKiTu = 0;
	//Doc block 8 bit:
	unsigned char c;

	while (1) {
		fread(&c, sizeof(char), 1, fpread);
		for (int i = 7; i >= 0; i--)
		{
			int ibit = getbit(c, i);
			if (ibit == 1)
				nCurrent = huffTree[nCurrent].nRight;
			else
				nCurrent = huffTree[nCurrent].nLeft;

			if (huffTree[nCurrent].nLeft == -1 && huffTree[nCurrent].nRight == -1)
			{

				fwrite(&huffTree[nCurrent].c, sizeof(char), 1, fdecode);

				isoKiTu++;
				if (isoKiTu == soKiTu)
				{
					fclose(fdecode);
					return;
				}

				nCurrent = nRoot;
			}

		}
		if (feof(fpread)) {
			break;
		}
	}

	fclose(fdecode);
}

void main() {
	// Nén du lieu
	NenHuffman("input.cpp", "output.txt");

	//Reset lai cau Huffman
	KhoiTao(); 

	//Doc file da nen va giai nen 
	FILE *fpread = fopen("output.txt", "rb");
	LoadHuffmanTree(fpread);
	XuatBangThongKe();

	//Tao cay huffman tu du lieu vua load
	int nRoot = TaoCayHuffman(); 
	XuatCayHuffman(nRoot, 0);
	PhatSinhMaBit(nRoot);
	XuatBangMaBit();

	// Giai nen du lieu
	GiaiNen(nRoot, fpread, "decompression.cpp");

	fclose(fpread);

}