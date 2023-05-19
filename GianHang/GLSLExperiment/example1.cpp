//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sach cac vector phap tuyen ung voi moi dinh*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_loc;

mat4 model, model_cuaso, model_cua, ban;
mat4 instance;

GLfloat yc = 0, xcs =0, zcs = 0;

GLfloat m = 0, n = 0, p = 0;
GLfloat aa = 0, bb = 0, cc = 0, dd = 0, goc = 0;
mat4 model_robot, model_tay, model_chan, model_ke;


GLuint view_loc;
GLfloat theta[] = { 0, 0, 0 };
GLfloat xx = 0, yy = 0, zz = 0;


GLuint projection_loc;
GLfloat l = -2, r = 2;
GLfloat bottom = -2, top = 2;
GLfloat zNear = 1, zFar = 10;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{

	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));
	normals[Index] = normal;  colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal;  colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal;  colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal;  colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal;  colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal;  colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
	
}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	view_loc = glGetUniformLocation(program, "View");
	projection_loc = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}
// Hàm để tô màu đối tượng
void to_mau(color4 color) {
	material_diffuse = color;  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}

// vẽ tường
void Tuong(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
{
	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(x, y, z) * RotateY(a) *  Scale(0.05, 5.0, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tam1() {
	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(-2.48, 0, -1.75) * Scale(0.05, 5.0, 1.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tam2(){
	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(-2.48, 0, 1.25) * Scale(0.05, 5.0, 2.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tamtren() {
	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(-2.48, 1.75, -0.5) * Scale(0.05, 1.5, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tamduoi() {
	material_diffuse = vec4(0.8, 0.8, 0.8, 0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(-2.48, -1.5, -0.5) * Scale(0.05, 2, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa sổ
void Cuaso() {
	material_diffuse = vec4(1.0, 0.0, 1.0, 1.0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(-2.48, 0.25, -0.5) * Scale(0.05, 1.5, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_cuaso * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Tuongtrai() {
	Tam1();
	Tam2();
	Tamtren();
	Tamduoi();

	model_cuaso = Translate(0, xcs, zcs);
	Cuaso();
}
//vẽ nền và mái 0.0, 0.0, 0.0, 1.0
void Nen(GLfloat x, GLfloat y, GLfloat z)
{

	material_diffuse = vec4(0.0, 0.0, 0.0, 1.0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(x, y, z) * RotateZ(90) * Scale(0.05, 5.0, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// trục xoay cửa cuốn 
void trucxoaycuacuon(GLfloat y)
{
	material_diffuse = vec4(0.0, 0.0, 0.0, 1.0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	instance = Translate(0, 2.35, 2.4) * RotateX(y) * Scale(5, 0.4, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Cua() {
	material_diffuse = vec4(0.0, 1.0, 1.0, 1.0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(0, 0, 2.48) * Scale(4.9, 4.9, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_cua * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);   
}
// bàn giữa phòng
void MatBan() {
	material_diffuse = vec4(1.0, 1.0, 1.0, 1.0);  // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(0, -1.6, 0) * Scale(1.2, 0.02, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void ChanBan(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(0, -1.6, 0) * Translate(x, y, z) * Scale(0.06, 0.8, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ThanBan() {
	MatBan();
	ChanBan(-0.57, -0.41, 0.27);
	ChanBan(-0.57, -0.41, -0.27);
	ChanBan(0.57, -0.41, 0.27);
	ChanBan(0.57, -0.41, -0.27);
}
void NganBan() {
	instance = Translate(0, -1.6, 0) * Translate(0, -0.02, 0) * Scale(0.54, 0.04, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void HopBan() {
	instance = Translate(0, -1.6, 0) * Translate(0.42, -0.2, 0) * Scale(0.25, 0.4, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Ban() {
	ThanBan();
	NganBan();
	HopBan();
}
void Phong() {

	Tuong(2.48, 0, 0, 0);
	Tuongtrai();
	Tuong(0, 0, -2.48, 90);
	Nen(0, 2.48, 0);
	Nen(0, -2.48, 0);

	for (int i = 0; i < 180; i++)
	{
		trucxoaycuacuon(i);
	}

	Cua();
	model_cua = Translate(0, 2.48, 0) * Scale(1, 1 + yc, 1) * Translate(0, -2.48, 0);

}
//robotHa
void than() {
	instance = Scale(0.3, 0.2, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void co() {
	instance = Translate(0, 0.115, 0) * Scale(0.05, 0.02, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void than_robot() {
	than();
	co();
}
void dau() {
	instance = Translate(0, 0.17, 0) * RotateY(aa) * Scale(0.15, 0.1, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void sung(GLfloat x, GLfloat y, GLfloat z) {
	instance = Translate(x, y, z) * RotateY(aa) * Scale(0.02, 0.03, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void dau_robot() {
	dau();
	sung(-0.03, 0.235, 0);
	sung(0.03, 0.235, 0);
}
void chantren() {
	instance = Scale(0.08, 0.1, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * model_chan * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanduoi() {
	instance = Scale(0.1, 0.03, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * model_chan * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void chantrai() {
	model_chan = Translate(-0.075, -0.15, 0) * RotateX(dd);
	chantren();
	model_chan = model_chan * Translate(0, -0.065, 0);
	chanduoi();
}
void chanphai() {
	model_chan = Translate(0.075, -0.15, 0) * RotateX(dd);
	chantren();
	model_chan = model_chan * Translate(0, -0.065, 0);
	chanduoi();
}
void chan_robot() {
	chantrai();
	chanphai();
}
void taytren() {
	instance = Scale(0.08, 0.1, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * model_tay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tayduoi() {
	instance = Translate(0, -0.05, 0) * Scale(0.04, 0.15, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model_robot * model_tay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taytrai() {
	model_tay = Translate(-0.19, 0.07, 0) * RotateX(bb);
	taytren();
	model_tay = model_tay * Translate(0, -0.07, 0) * RotateX(cc);
	tayduoi();
}
void tayphai() {
	model_tay = Translate(0.19, 0.07, 0) * RotateX(bb);
	taytren();
	model_tay = model_tay * Translate(0, -0.07, 0) * RotateX(cc);
	tayduoi();
}
void tay_robot() {
	taytrai();
	tayphai();
}
void robot1() {
	model_robot = Translate(m, n, p) * Translate(1.2, -1.7, -1.5) * RotateY(goc)* RotateY(180) * Scale(2, 2, 2);
	than_robot();
	chan_robot();
	dau_robot();
	tay_robot();
}

void ke1() {
	mat4 instance = Translate(1.2, -2.3, -1.5) * Scale(0.7, 0.3, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model* model_ke * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat zE = 4.5;
void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );                

	Phong();
	//Tuongtrai();
	Ban();
	robot1();
	ke1();

	model = RotateX(xx) * RotateY(yy) * RotateZ(zz);

	vec4 eye = vec4(0, 0, zE, 1);
	vec4 at = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 1);

	mat4 View = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, View);

	mat4 Projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, Projection);

	glutSwapBuffers();									   
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
	case 'a': // đi vào
		zE *= 0.9;
		glutPostRedisplay();
		break;
	case 'A':
		zE *= 1.1;
		glutPostRedisplay();
		break;
	case 'x':
		xx += 5;
		if (xx > 360) xx -= 360;
		glutPostRedisplay();
		break;
	case 'X':
		xx -= 5;
		if (xx > 360) xx -= 360;
		glutPostRedisplay();
		break;
	case 'y':
		yy += 5;
		if (yy > 360) yy -= 360;
		glutPostRedisplay();
		break;
	case 'Y':
		yy -= 5;
		if (yy > 360) yy -= 360;
		glutPostRedisplay();
		break;
	case 'z':
		zz += 5;
		if (zz > 360) zz -= 360;
		glutPostRedisplay();
		break;
	case 'Z':
		zz -= 5;
		if (zz > 360) zz -= 360;
		glutPostRedisplay();
		break;
	case '1': // mở của cuốn
		yc -= 0.1;
		if (yc <= -0.9) { yc = -0.9; }
		glutPostRedisplay();
		break;
	case '2': // đóng cửa cuốn
		yc += 0.1;
		if (yc >= 0) { yc = 0; }
		glutPostRedisplay();
		break;
	case '3': // mở cửa sổ
		xcs = 0.05;
		zcs += 0.1;
		if (zcs >= 1) { 
			zcs = 1;
			xcs = 0.05;
		}
		glutPostRedisplay();		
		break;
	case '4': // đóng cửa sổ
		zcs -= 0.1;
		if (zcs <= 0) {
			zcs =0;
			xcs = 0;
		}
		glutPostRedisplay();
		break;
	//robotHa
	case 'T':
		m += -1.2;
		n += 0.55;
		p += 1.5;
		glutPostRedisplay();
		break;
	case 't':
		m += 1.2;
		n += -0.55;
		p += -1.5;
		glutPostRedisplay();
		break;
	case 'M': // quay robot
		goc = goc + 30;
		glutPostRedisplay();
		break;
	case 'm':
		goc = goc - 30;
		glutPostRedisplay();
		break;
	case 'D': //quay dau
		aa = aa + 5;
		if (aa > 90)
			aa = 90;
		glutPostRedisplay();
		break;
	case 'd':
		aa = aa - 5;
		if (aa < -90)
			aa = -90;
		glutPostRedisplay();
		break;
	case 'U': //upper arm
		bb = bb + 5;
		if (bb >= 180)
			bb = 180;
		glutPostRedisplay();
		break;
	case 'u':
		bb = bb - 5;
		if (bb <= -30)
			bb = -30;
		glutPostRedisplay();
		break;
	case 'L': //lower arm
		cc = cc + 5;
		if (cc > 90)
			cc = 90;
		glutPostRedisplay();
		break;
	case 'l':
		cc = cc - 5;
		if (cc < 0)
			cc = 0;
		glutPostRedisplay();
		break;
	case 'C': //chan
		dd = dd + 5;
		if (dd > 90)
			dd = 90;
		glutPostRedisplay();
		break;
	case 'c':
		dd = dd - 5;
		if (dd < -30)
			dd = -30;
		glutPostRedisplay();
		break;
    }
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
