#include<GL/glut.h>
#include<iostream>
#include<string>
#include<algorithm>
#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>
#define _USE_MATH_DEFINES
#include<math.h>

using namespace std;

struct Color
{
	GLint r=0;
	GLint g=0;
	GLint b=255;
};

struct Color glob;

//global variables
int width = 600;
int height = 600;
const char* name = "test.ppm";

//color
GLubyte R = 0, B = 0, G = 0;
int colorIncrementer = 25;

//state variables
bool isMousePressed = false;

//tool states
bool isPenToolActive = false;
bool isKaleidoscope = false;
int symmetryLines = 30;
bool doubleSymmetry = true;
bool isLineToolActive = false;
bool isRectangleToolActive = false;
bool isCircleToolActive = false;
bool diameterTool = false;
bool isFillActive = false;
bool isSave = false;

//hold variables
bool isShiftPressed = false;

//Window_display
void InitWindow();
void display(void);
void CreateMyMenu();

//contoller
void keyboard(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void specialFunc(int, int, int);
void specialUpFunc(int, int, int);
void mouseFunc(int, int, int, int);
void motionFunc(int, int);
void passiveMotionFunc(int, int);

//tools
void PenTool(int, int, int, int);
void LineTool(int, int, int, int);
void RectangleTool(int, int, int, int);
void CircleTool(int, int, int, int);
void ChangeColorUp();
void ChangeColorDown();
void Flood(GLint x1, GLint y1);
void Save();

//drawing
void RecordPoints(int, int);
void SetColor(GLubyte, GLubyte, GLubyte);


//Ansilary drawing
void ToggleTools(bool&);
void SnapLine(int, int, int, int);
void RotatePoint(int&, int&, int, int, double);

//menu
void MainMenu(int);
void ToolSelectorMenu(int);
void ColorSelectorMenu(int);

//Drawing Algorithms
void DrawLine(int x1, int y1, int x2, int y2);
void DrawCircle(int h, int k, int radius);
double CalculateEpsilon(int r);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	InitWindow();

	glutPassiveMotionFunc(passiveMotionFunc);//capture free moment of the mouse
	glutMotionFunc(motionFunc);//capture hold state of the mouse
	glutMouseFunc(mouseFunc);//capture clicks
	glutKeyboardFunc(keyboard);//capture key pressed
	glutKeyboardUpFunc(keyboardUp);		//capture released keys
	glutSpecialFunc(specialFunc);
	glutSpecialUpFunc(specialUpFunc);			//capture released special keys
	glutDisplayFunc(display);

	CreateMyMenu();

	glutMainLoop();
}

void InitWindow()											//-----INIT_WINDOW-----
{
	//create window
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("PAINT");

	//initialise canvas
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, width, 0, height);		//set the projection area
	glClearColor(1, 1, 1, 1);					//set color of the screen
	glClear(GL_COLOR_BUFFER_BIT);			//clear the buffer of the screen
	glFlush();								//force update of the screen

	//set pencil color
	glColor3ub(0, 0, 0);
}

void display(void)											//-----DISPLAY-----
{
	//glClear(GL_COLOR_BUFFER_BIT);			//clear the buffer of the screen
	glFlush();								//force update of the screen
}

void CreateMyMenu()											//-----MENU-----
{
	//Creating submenus
	int subMenuTool = glutCreateMenu(ToolSelectorMenu);	    //Tool_selctor_tool
	glutAddMenuEntry("PEN TOOL", 1);
	glutAddMenuEntry("LINE TOOL", 2);
	glutAddMenuEntry("RECTANGLE TOOL", 3);
	glutAddMenuEntry("CIRCLE TOOL", 4);
	glutAddMenuEntry("FILL", 5);
	glutAddMenuEntry("SAVE", 6);

	int subMenuColor = glutCreateMenu(ColorSelectorMenu);	//Color_Menu
	glutAddMenuEntry("RED", 1);
	glutAddMenuEntry("BLUE", 2);
	glutAddMenuEntry("BLACK", 3);
	glutAddMenuEntry("WHITE", 4);
	glutAddMenuEntry("YELLOW", 5);
	glutAddMenuEntry("GREEN", 6);
	glutAddMenuEntry("DEEP_PINK", 7);
	glutAddMenuEntry("AQUAMARINE", 8);


	//Creating Main Menu
	glutCreateMenu(MainMenu);							//Tool_selctor_tool
	glutAddSubMenu("TOOLS", subMenuTool);
	glutAddSubMenu("COLORS", subMenuColor);
	glutAddMenuEntry("EXIT", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//=======================================CONTROLLER====================================

void keyboard(unsigned char key, int x, int y)				//-----KEYBOARD-----
{
	switch (key)
	{
	case 'd'://clear the screen
		glClear(GL_COLOR_BUFFER_BIT);						//clear the buffer of the screen
		glFlush();
		break;
	case 's'://set the color
		cout << "ENTER COLOR" << endl;
		double r, b, g;
		cin >> r >> b >> g;
		SetColor(r, b, g);
		break;

	case 'p'://toggle PencilTool on/off
		ToggleTools(isPenToolActive);
		break;

	case 'l'://toggle LineTool on/off
		ToggleTools(isLineToolActive);
		break;

	case 'r'://toggle Rectangle tool
		ToggleTools(isRectangleToolActive);
		break;

	case 'c'://toggle Circle tool
		ToggleTools(isCircleToolActive);
		break;
	case '+'://increase symmetry
		if (isKaleidoscope && symmetryLines < 360)
		{
			symmetryLines++;
			cout << "SYMMETRY: " << symmetryLines << endl;
		}
		break;
	case '-'://decrease symmetry
		if (isKaleidoscope && symmetryLines > 2)
		{
			symmetryLines--;
			cout << "SYMMETRY: " << symmetryLines << endl;

		}
		break;

	case '1'://multifunction
		if (isPenToolActive)//toggle kaleidoScope if pen is selected
		{
			isKaleidoscope = !isKaleidoscope;
			cout << "KALEIIDOSCOPE: " << isKaleidoscope << endl;
		}
		else if (isCircleToolActive)
		{
			diameterTool = !diameterTool;
			cout << "DIAMETER CIRCLE: " << diameterTool << endl;
		}
		break;
	case '2':
		if (isKaleidoscope)
		{
			doubleSymmetry = !doubleSymmetry;
			cout << "DOUBLE SYMMETRY: " << doubleSymmetry << endl;
		}
	}
}

void mouseFunc(int button, int state, int x, int y)			//-----mouseFunc-----
{
	if (state == GLUT_DOWN)
	{

		isMousePressed = true;
		if (isLineToolActive || isRectangleToolActive || isCircleToolActive || isFillActive)
			RecordPoints(x, -y + height);

	}
	else if (state == GLUT_UP)
	{
		//cout << "MOUSE IS RELEASED" << endl;
		isMousePressed = false;
		if (isPenToolActive)
		{
			RecordPoints(0, 0);//to reset penTool so i can lift up my pen
		}
	}
	if (button == 3)//Scroll Up
	{
		ChangeColorUp();
	}
	else if (button == 4)//Scroll Down
	{
		ChangeColorDown();
	}
}

void motionFunc(int x, int y)								//-----motionFunc-----
{
	if (isMousePressed && isPenToolActive)//only draw on mouse if pentool is active
	{
		RecordPoints(x, -y + height);
	}
}

void passiveMotionFunc(int x, int y)						//-----passiveMotionFunc-----
{
	//cout << "*";
}

void keyboardUp(unsigned char key, int x, int y)			//-----relasing of keyboard_inputs-----
{
	//cout << "key released" << key << endl;
}

void specialFunc(int key, int x, int y)						//-----pressing of special keys-----
{
	switch (key)
	{
	case 112://shift is pressed
		isShiftPressed = true;
		break;
	}
	//cout << key << endl;
}

void specialUpFunc(int key, int x, int y)					//-----releasing of special keys-----
{
	switch (key)
	{
	case 112://shift is released
		isShiftPressed = false;
		break;
	}
}


//=============================CONTROLLER_END======================================

void RecordPoints(int x, int y)							 	//-----RECORD_POINTS-----
{
	static int count = 0;
	static int x1, y1;

	if (!isMousePressed)	//so  i can lift my pen up
	{
		count = 0;
		return;
	}

	if (count == 0)//record first point
	{
		x1 = x;
		y1 = y;
		count++;
	}
	else//record second point
	{
		if (isPenToolActive)
		{
			PenTool(x1, y1, x, y);
		}
		else if (isLineToolActive)
		{
			LineTool(x1, y1, x, y);
		}
		else if (isRectangleToolActive)//draw rectangle
		{
			RectangleTool(x1, y1, x, y);
		}
		else if (isCircleToolActive)
		{
			//cerr << "circle" << endl;
			CircleTool(x1, y1, x, y);
		}
		else if (isFillActive)
		{
			Flood(x1, y1);
		}


		if (!isPenToolActive)//reset points
		{
			count = 0;
		}
		else//pen tool is active or recording is on
		{
			x1 = x;
			y1 = y;
		}
	}

	glFlush();
}

//===========================ANSILARRY_DRAWING==============================

void SetColor(GLubyte r, GLubyte b, GLubyte g)			    //SetColor
{
	glColor3ub(r, b, g);
}

void ToggleTools(bool& current)							    //ToggleTools
{
	system("CLS");
	bool temp = current;
	isCircleToolActive = isRectangleToolActive = isKaleidoscope = isPenToolActive = isLineToolActive = isFillActive =false;
	current = !temp;

	cout << "LINETOOL : " << isLineToolActive << endl;
	cout << "PENTOOL  : " << isPenToolActive << endl;
	cout << "RECTANGLE: " << isRectangleToolActive << endl;
	cout << "CIRCLE   : " << isCircleToolActive << endl;
	cout << "FILL     : " << isFillActive << endl;
	cout << "SAVE     : " << isSave << endl;

	if (isSave) {
		Save();
	}
	isSave = false;
}

void SnapLine(int x1, int y1, int x2, int y2)
{
	int dy = abs(y2 - y1);
	int dx = abs(x2 - x1);

	if (dx > dy)//snap horizontally
	{
		DrawLine(x1, y1, x2, y1);
	}
	else       //snap vertically
	{
		DrawLine(x1, y1, x1, y2);
	}
}

void RotatePoint(int& X_new, int& Y_new, int x, int y, double angle)		//Rotate Point Around Centre
{
	double rad = (M_PI / (double)180) * angle;
	//cerr << rad << endl;
	int Tx = width / 2;
	int Ty = height / 2;

	X_new = x * cos(rad) + y * sin(rad) - Tx * cos(rad) - Ty * sin(rad) + Tx;
	Y_new = -x * sin(rad) + y * cos(rad) + Tx * sin(rad) - Ty * cos(rad) + Ty;
}
//==============================ANISILARY_DRAWING_END===========================

//====================================TOOLS=====================================

void PenTool(int x1, int y1, int x2, int y2)				//<+ PEN_TOOL +>
{
	DrawLine(x1, y1, x2, y2);
	//reafactor this
	if (isKaleidoscope)//4 symmetry
	{
		if (doubleSymmetry)
		{
			DrawLine(-x1 + width, y1, -x2 + width, y2);
		}
		int x3, y3, x4, y4;
		double angle = (double)360 / symmetryLines;
		double angleInc = angle;
		for (int i = 0; i < symmetryLines - 1; i++)
		{

			RotatePoint(x3, y3, x1, y1, angle);
			RotatePoint(x4, y4, x2, y2, angle);
			DrawLine(x3, y3, x4, y4);

			if (doubleSymmetry)
			{
				DrawLine(-x3 + width, y3, -x4 + width, y4);
			}
			angle += angleInc;
		}
	}
}

void LineTool(int x1, int y1, int x2, int y2)				//<+- LINE_TOOL -+>
{
	if (isShiftPressed)
	{
		SnapLine(x1, y1, x2, y2);
	}
	else
	{
		DrawLine(x1, y1, x2, y2);
	}
}

void RectangleTool(int x1, int y1, int x2, int y2)			//<+- RECTANGLE_TOOL -+>
{
	DrawLine(x1, y1, x1, y2);
	DrawLine(x1, y2, x2, y2);
	DrawLine(x2, y2, x2, y1);
	DrawLine(x2, y1, x1, y1);
}

void CircleTool(int x1, int y1, int x2, int y2)
{
	double radius = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

	if (diameterTool)
	{
		double h = (x1 + x2) / 2;
		double k = (y1 + y2) / 2;
		DrawCircle(h, k, radius / 2);
	}
	else
	{
		DrawCircle(x1, y1, radius);
	}
}

void ChangeColorUp()										//<+- COLOR_CHANGER_TOOL -+>
{
	R += colorIncrementer;
	if (R > 240)
	{
		B += colorIncrementer;
		if (B > 240)
			G += colorIncrementer;
	}
	//cout << (int)R << "," << (int)B << "," << (int)G << endl;
	SetColor(R, B, G);
}
void ChangeColorDown()										//<+- COLOR_CHANGER_TOOL -+>
{
	if (G < 10)
	{
		if (B < 10)
			R -= colorIncrementer;
		B -= colorIncrementer;
	}

	G -= colorIncrementer;
	//cout << (int)R << "," << (int)B << "," << (int)G << endl;

	SetColor(R, B, G);
}
//===============================TOOLS_END=======================================

//==================================MENU=========================================
void MainMenu(int  choice)					    //-----Simple_MENU-----
{
	switch (choice)
	{
	case 1://EXIT
		exit(0);
		break;
	}
}
void ToolSelectorMenu(int choice)				//-----Tool_Selector_Menu-----
{
	switch (choice)
	{
	case 1:
		ToggleTools(isPenToolActive);
		break;
	case 2:
		ToggleTools(isLineToolActive);
		break;
	case 3:
		ToggleTools(isRectangleToolActive);
		break;
	case 4:
		ToggleTools(isCircleToolActive);
		break;
	case 5:
		ToggleTools(isFillActive);
		break;
	case 6:
		ToggleTools(isSave);
	}
}

void ColorSelectorMenu(int choice)					//-----ColorSelector-----
{
	switch (choice)
	{
	case 1://RED
		SetColor(255, 0, 0);
		glob.r = 255;
		glob.g = 0;
		glob.b = 0;
		break;
	case 2://BLUE
		SetColor(0, 0, 255);
		glob.r = 0;
		glob.g = 0;
		glob.b = 255;
		break;
	case 3://BLACK
		SetColor(0, 0, 0);
		glob.r = 0;
		glob.g = 0;
		glob.b = 0;
		break;
	case 4://WHITE
		SetColor(255, 255, 255);
		glob.r = 255;
		glob.g = 255;
		glob.b = 255;
		break;
	case 5://YELLOW
		SetColor(255, 255, 0);
		glob.r = 255;
		glob.g = 255;
		glob.b = 0;
		break;
	case 6://GREEN
		SetColor(0, 128, 0);
		glob.r = 0;
		glob.g = 128;
		glob.b = 0;
		break;
	case 7://DEEPPINK
		SetColor(255, 20, 147);
		glob.r = 255;
		glob.g = 0;
		glob.b = 147;
		break;
	case 8://AQUAMARINE
		SetColor(127, 255, 212);
		glob.r = 0;
		glob.g = 255;
		glob.b = 190;
		break;
	}
}



//================================DRAWING_ALGORITHMS==============================

void DrawLine(int x1, int y1, int x2, int y2)				//-----DDA-----
{
	double dx = x2 - x1,
		dy = y2 - y1;
	int step = max(abs(dx), abs(dy));
	double xinc = dx / step,
		yinc = dy / step;
	double x = x1,
		y = y1;

	while (step--)
	{
		x += xinc;
		y += yinc;
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
	}
}


void DrawCircle(int h, int k, int radius)					//-----DDA_CIRCLE-----
{
	double x = radius,
		y = 0;
	double eps = CalculateEpsilon(radius);
	//cerr << "eps: " << eps << endl;

	while ((y - 0) < eps || (radius - x) > eps)
	{
		glBegin(GL_POINTS);
		glVertex2f(x + h, y + k);
		glEnd();
		//putpixel(x + h, y + k, color);
		x += y * eps;
		y -= x * eps;
	}
}


double CalculateEpsilon(int r)							   //CALCULATE_EPSILON
{
	int n;
	for (n = 0; pow(2, n) < r; n++);
	return (double)1 / pow(2, n);
}

Color getPixelColor(GLint x, GLint y)
{
	Color color;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &color);
	cout << 1 << endl;
	return color;
}
void setPixelColor(GLint x, GLint y, Color color)
{
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
	cout << 2 << endl;
}

void Flood(GLint x, GLint y) {
	Color color;
	Color newColor;
	newColor.r = glob.r;
	newColor.g = glob.g;
	newColor.b = glob.b;
	//newColor.r = 0;
	//newColor.g = 0;
	//newColor.b = 255;
	Color oldColor;
	oldColor.r = 1065353216;
	oldColor.g = 1065353216;
	oldColor.b = 1065353216;
	color = getPixelColor(x, y);
	cout << color.r << " " <<color.g<< " " <<color.b<<endl;
	if (color.r == oldColor.r && color.g == oldColor.g && color.b == oldColor.b)
	{
		int count=0;
		setPixelColor(x, y, newColor);
		Flood(x + 1, y);
		Flood(x, y + 1);
		Flood(x - 1, y);
		Flood(x, y - 1);
		cout << count << endl;
	}
	return;
}

void Save() {
	cout << "Enter name of file: " << endl;
	string name;
	cin >> name;
	name += ".tga";
	const char* filepath = name.c_str();
	int* buffer = new int[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);

	FILE* out = fopen(filepath, "w");
	short  TGAhead[] = { 0, 2, 0, 0, 0, 0, width, height, 24 };
	fwrite(&TGAhead, sizeof(TGAhead), 1, out);
	fwrite(buffer, 3 * width * height, 1, out);
	fclose(out);
	cout << "File Saved Successfully!!" << endl;
}