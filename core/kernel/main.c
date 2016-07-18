#define VID_MEM 0xB8000
int main() {
	unsigned char* p = (unsigned char*)VID_MEM;
	*p++ = 'Z';
	*p = 15;
	return 0;
}
