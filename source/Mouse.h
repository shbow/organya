typedef struct MouseData{
	private:
		POINT point;
	public:
		void GetMousePosition(long *x,long *y);
}MOUSEDATA;
extern MOUSEDATA mouse_data;

