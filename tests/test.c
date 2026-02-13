struct Vga
{
	int height;
	int width;
	int depth;
};


int Vga_get_height(struct Vga *self)
{
	return self->height;
}

enum Color
{
	Color_RED = 7,
	Color_GREEN,
	Color_BLUE,
};

int puts(char  *__c4internal_str_c4_tmp_0);
int main()
{
	enum Color color = Color_BLUE;
	struct Vga vga = (struct Vga){
		.depth = 2,
		.height = 25,
		.width = 80,
	};

	struct Vga *vga_ptr = &vga;
	puts("hi");
	return Vga_get_height(&vga);
}
