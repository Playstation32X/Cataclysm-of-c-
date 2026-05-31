#include<iostream> 
#include<cstdlib>
#define GLEW_STATIC
#include<glew.h>
#include<GL/glut.h>  
#include<glu.h> 
#include<glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>
#include<glm/vector_relational.hpp>
#include<stdio.h>    
#include<windows.h> 
#include<mmsystem.h>
#define rads 3.14159/180
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION  
#include<tiny_obj_loader.h>
#include<vector>  
#include<string.h>  
#include<math.h>  

#define SW 700 
#define SH 500 
GLfloat l0_diffuse[] = { 1,1,1,0 };
GLfloat specular[] = { 1,1,1 };
GLfloat shiny[] = { 128 };
GLfloat ambeint[] = { 0.1,0.1,0.1 };
float UniTime;
float px, py, pz, prx,pry; 
float vx, vy, vz;
float oldpx; 
float oldpy;
float oldpz; 
bool onobject,Move=true,gravity; 
float speed = 0.3; 
int wallTex;
std::vector<std::vector<int>>map;

std::string line;
GLuint texs[37]; 
GLuint animatedGuyTexs[5];
GLuint aluminiumGuyTexs[5];
int tool=0;
bool camMove = false;

bool cubecollide(float px, float py, float pz, float x1, float x2, float y1, float y2, float z1, float z2)
{
	return(px > x1 && px<x2 && py>y1 && py<y2 && pz>z1 && pz < z2);
}

bool ifdist(float x1, float x2, float y1, float y2, float z1, float z2, float threshold)
{
	float dx = x1 - x2; 
	float dy = y1 - y2; 
	float dz = z1 - z2; 
	float dist = sqrt(dx*dx+dy*dy+dz*dz);
	return(dist < threshold);
}

GLuint texload(const char* file)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int texw, texh, channels;
	unsigned char* filedata = stbi_load(file, &texw, &texh, &channels, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!filedata)
	{
		exit(1);
	}
	GLenum format = (channels == 4) ? GL_RGBA : (channels == 3) ? GL_RGB : (channels == 1) ? GL_RED : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, texw, texh, 0, format, GL_UNSIGNED_BYTE, filedata);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

	stbi_image_free(filedata);
	return textureID;
} 

struct wall
{
	float wx, wy, wz;
    int tex;
    const char* type;
	int scalex, scaley, scalez; 
      std::string doordir;
	  float WX, WY, WZ;
	void WallDraw()
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texs[tex]);
		glPushMatrix();
		float distx = px - wx;
		float distz = pz - wz;
		float disty = py - wy;
		float dist = sqrt(distx * distx + disty * disty + distz * distz);
	

		if (tex == 11)
		{
			if (dist < scaley)
			{
				wx = WX;
				if (doordir == "up")
				{
					WY += 0.1;
				}
				else if (doordir == "down")
				{
					WY -= 0.1;
				}
				if (doordir == "right")
				{
					WX += 0.1;
				}
				else if (doordir == "left")
				{
					WX -= 0.1;
				}
				if (doordir == "forward")
				{
					WZ += 0.1;
				}
				else if (doordir == "backward")
				{
					WZ -= 0.1;
				}
			}
			else
			{
				WX = wx;
				WY = wy;
				WZ = wz;
			}
			glTranslatef(WX, WY, WZ);
		}
		else
		{
			glTranslatef(wx, wy, wz);
		}

		glColor3f(1, 1, 1);
		glScalef(scalex, scaley, scalez);
		if (type == "X")
		{
			glBegin(GL_QUADS);
			glTexCoord2i(0, 1); glVertex3f(-1, -1, 0);
			glTexCoord2i(0, 0); glVertex3f(-1, 1, 0);
			glTexCoord2i(1, 0); glVertex3f(1, 1, 0);
			glTexCoord2i(1, 1); glVertex3f(1, -1, 0);
			glEnd();

			if (cubecollide(px, py, pz, wx - scalex, wx + scalex, wy - scaley, wy + scaley, wz - 0.3, wz + 0.3))
			{
				std::cout << "WALL HERE\n";
				px = oldpx;
				pz = oldpz; 
			}
		}
		if (type == "Z")
		{
			glBegin(GL_QUADS);
			glTexCoord2i(0, 1); glVertex3f(0, -1, -1);
			glTexCoord2i(0, 0); glVertex3f(0, 1, -1);
			glTexCoord2i(1, 0); glVertex3f(0, 1, 1);
			glTexCoord2i(1, 1); glVertex3f(0, -1, 1);
			glEnd();

			if (cubecollide(px, py, pz, wx - 0.3, wx + 0.3, wy - scaley, wy + scaley, wz - scalez, wz + scalez))
			{
				std::cout << "WALL HERE\n";
				pz = oldpz;
				px = oldpx;
			}
		}

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
}; 
std::vector<wall>wallz;

struct tile
{
	float fx, fy, fz; 
	float scalex, scaley, scalez; 
	GLuint tex;
	void Floor()
	{
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex);


		glTranslatef(fx, fy, fz);
		glScalef(scalex, scaley, scalez);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3f(-1.0, 0, -1.0);
		glTexCoord2f(0, 1);
		glVertex3f(1.0, 0, -1.0);
		glTexCoord2f(1, 1);
		glVertex3f(1.0, 0, 1.0);

		glTexCoord2f(1, 0);
		glVertex3f(-1.0, 0, 1.0);
		glEnd();
		if (cubecollide(px, py, pz, fx - (scalex), fx + (scalex), fy - 0.1, fy + 3, fz - (scalez), fz + (scalez)))
		{
			onobject = true;
			if (tex == texs[5])
			{
				vy += 2;
			}
		}
		if (cubecollide(px, py, pz, fx - (scalex), fx + (scalex), fy - 0.1, fy, fz - (scalez), fz + (scalez)))
		{
			py = oldpy;
		}
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

};
std::vector<tile>tilez; 

struct playermove
{
	bool up, down, left, right,upward,downward,lookleft,lookright,lookup,lookdown;
};
playermove move = {false, false, false, false,false,false,false,false,false,false}; 

struct model
{
	std::vector<tinyobj::real_t>vertix;
	std::vector<tinyobj::real_t>normals; 
	GLuint vbos[2];
};
model coolBoy,hammer;
void OBJloader(model &M,std::string path)
{
	tinyobj::attrib_t atrib;
	std::vector<tinyobj::shape_t>shapes;
	std::vector<tinyobj::material_t>mats;

	bool retun = tinyobj::LoadObj(&atrib, &shapes, &mats, NULL, NULL, path.c_str());
	M.vertix.clear();
	M.normals.clear();
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			int vIdx = 3 * index.vertex_index;
			M.vertix.push_back(atrib.vertices[vIdx + 0]);
			M.vertix.push_back(atrib.vertices[vIdx + 1]);
			M.vertix.push_back(atrib.vertices[vIdx + 2]);
			if (index.normal_index >= 0)
			{
				int nIdx = 3 * index.normal_index;
				M.normals.push_back(atrib.normals[nIdx + 0]);
				M.normals.push_back(atrib.normals[nIdx + 1]);
				M.normals.push_back(atrib.normals[nIdx + 2]);
			}
			else
			{
				M.normals.push_back(0);
				M.normals.push_back(1);
				M.normals.push_back(0);
			}
			
		}
	}
	glGenBuffers(2, M.vbos);

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, M.vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, M.vertix.size() * sizeof(float), M.vertix.data(), GL_STATIC_DRAW);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER,M.vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, M.normals.size() * sizeof(float), M.normals.data(), GL_STATIC_DRAW);
}
void OBJdrawer(model M, float x, float y, float z, float angle, float scale,const char* color,float transparency)
{
	glPushMatrix();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glTranslatef(x,y,z);
	glRotatef(angle, 0, 1, 0);
	glScalef(scale, scale, scale); 
	if(color=="red")
	glColor4f(1, 0, 0, transparency);
	if (color == "green")
		glColor4f(0, 1, 0, transparency);
	if (color == "blue")
		glColor4f(0, 0, 1, transparency);
	if (color == "grey")
		glColor4f(0.3, 0.3, 0.3, transparency);
	if (color == "yellow")
		glColor4f(1, 1, 0, transparency); 
	if (color == "orange")
		glColor4f(1,0.5,0, transparency); 
	if (color == "purple")
		glColor4f(1,0,1, transparency);
	glBindBuffer(GL_ARRAY_BUFFER, M.vbos[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, M.vbos[1]);
	glNormalPointer(GL_FLOAT, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, M.vertix.size() / 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();
} 
struct tools
{
	float tx, ty, tz; 
	int toolCode=0;
	model toolmodel;  
	bool collected = false; 
	void toolMangager()
	{ 
		//collision size
		int CS = 3;
		if (cubecollide(px, py, pz, tx - CS, tx + CS, ty - CS, ty + CS, tz - CS, tz + CS))
		{
			collected = true;
			tool = toolCode;  
		}
		if(!collected==true)
		OBJdrawer(toolmodel,tx,ty,tz,UniTime,0.5,"grey",1);
	}
};
std::vector<tools>toolz; 
void Sprite(float sx, float sy, float sz, float scale, GLuint SpriteTex)
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, SpriteTex);

	glTranslatef(sx, sy, sz);
	glRotatef(pry, 0, 1, 0);
	glScalef(scale, scale, scale);
	glColor4f(1, 1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
	glTexCoord2f(1, 0); glVertex3f(-1, 1, 0);
	glTexCoord2f(1, 1); glVertex3f(-1, -1, 0);
	glTexCoord2f(0, 1); glVertex3f(1, -1, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Player()
{  
	oldpx = px;  
	oldpy = py;
	oldpz = pz;
		if (move.up == true)
		{
			px += cos((pry + 90) * rads) * speed;
			pz -= sin((pry + 90) * rads) * speed;
		}
		if (move.down == true)
		{
			px += cos((pry - 90) * rads) * speed;
			pz -= sin((pry - 90) * rads) * speed;
		}
		if (move.left == true)
		{
			px += cos((pry + 180) * rads) * speed;
			pz -= sin((pry + 180) * rads) * speed;
		}
		if (move.right == true)
		{
			px += cos((pry)*rads) * speed;
			pz -= sin((pry)*rads) * speed;
		}
		if (move.lookleft == true)
		{
			pry += 1;
		}
		if (move.lookright == true)
		{
			pry -= 1;
		}
		if (move.lookup == true)
		{
			prx += 1;
		}
		if (move.lookdown == true)
		{
			prx -= 1;
		}
		if (move.upward == true)
		{
			py += speed;
		}
		if (py < -12)
		{
			px = 0;
			py = 3;
			pz = 0;
	    }
		
			switch (tool)
			{
			case(1):  
				OBJdrawer(hammer,2,-1,-2.5,180,0.2,"grey",1);
				break;
			}
	
	glRotatef(-prx,1,0,0);  
	glRotatef(-pry,0,1,0); 
	glTranslatef(-px,-py,-pz);	
}

void keys1(unsigned char key,int,int)
{
	if (key == 'w')
	{
		move.up = true;
	} 
	if (key == 's')
	{
		move.down = true;
	} 
	if (key == 'a')
	{
		move.left = true;
	} 
	if (key == 'd')
	{
		move.right = true;
	}     
	if (key == 'p')
		exit(1);
	if (key == 32)
		move.upward = true; 
	if (key == 8)
	{
		if (!wallz.empty())
		{
			wallz.pop_back();
		}
		
	}
	if (key == 0x7f)
	{
		if (tilez.size() > 0)
			tilez.pop_back();
	}
	switch (key)
	{
	 case('1'):
		 wallTex = 1;  
		 std::cout << "1\n";
	 break;
	 case('2'):
		 wallTex = 2;
		 break; 
	 case('3'):
		 wallTex = 3; 
		 break;
	 case('4'):
		 wallTex = 4; 
		 break; 
	 case('5'):
		 wallTex = 5;
		 break;
	 case('6'):
		 wallTex = 6;
		 break;
	 case('7'):
		 wallTex = 7;
		 break;
	 case('8'):
		 wallTex = 8;
		 break;
	 case('9'):
		 wallTex = 9;
		 break;
	 case('0'):
		 wallTex = 10;
		 break;
	} 
	glutPostRedisplay();
}
void special(int key, int, int)
{
	if (key == GLUT_KEY_UP)
	{
		move.lookup = true;
	}
	if (key == GLUT_KEY_DOWN)
	{
		move.lookdown = true;
	}
	if (key == GLUT_KEY_LEFT)
	{
		move.lookleft = true;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		move.lookright = true;
	}  
	if (key == GLUT_KEY_HOME)
		camMove = true;
	if (key == GLUT_KEY_END)
		camMove = false;
}
void notsospecial(int key, int, int)
{
	if (key == GLUT_KEY_UP)
	{
		move.lookup = false;
	}
	if (key == GLUT_KEY_DOWN)
	{
		move.lookdown = false;
	}
	if (key == GLUT_KEY_LEFT)
	{
		move.lookleft = false;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		move.lookright = false;
	}
}
void keys2(unsigned char key, int, int)
{
	if (key == 'w')
	{
		move.up = false;
	}
	if (key == 's')
	{
		move.down = false;
	}
	if (key == 'a')
	{
		move.left = false;
	}
	if (key == 'd')
	{
		move.right = false;
	}
	if (key == 'e')
	{
		move.upward = false;
	}
	if (key == 'q')
	{
		move.downward = false;
	} 
	if (key == 32)
		move.upward = false;
}
void Floor(float fx, float fy, float fz, float scalex, float scaley,float scalez, GLuint tex)
{ 
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	

	glTranslatef(fx, fy, fz);
	glScalef(scalex, scaley, scalez);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);


	glTexCoord2f(0, 0);

	glVertex3f(-20.0, 0, -20.0);

	glTexCoord2f(0, 1);

	glVertex3f(20.0, 0, -20.0);

	glTexCoord2f(1, 1);

	glVertex3f(20.0, 0, 20.0);

	glTexCoord2f(1, 0);
	glVertex3f(-20.0, 0, 20.0);
	glEnd();
	if (cubecollide(px, py, pz, fx - (20 * scalex), fx + (20 * scalex), fy - 0.25, fy + 3, fz - (20 * scalez), fz + (20 * scalez)))
	{
		std::cout << "floor is here\n";
		onobject = true;
		if (tex == texs[5])
		{
			vy += 2;
		}
	}  
	if (cubecollide(px, py, pz, fx - (20*scalex), fx + (20*scalex), fy - 0.5, fy, fz - (20*scalez), fz + (20*scalez)))
	{
		py = oldpy; 
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void WallDraw(float wx, float wy, float wz, int type, float scalex,float scaley,float scalez, int tex,const char* doordir)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texs[tex]); 
	glPushMatrix(); 
	float distx = px - wx;
	float distz = pz - wz; 
	float disty = py - wy;
	float dist = sqrt(distx*distx + disty*disty + distz*distz);   
	static float WX=wx;
	static float WY=wy;
	static float WZ=wz;

	if (tex == 11)
	{
		if (dist < scaley)
		{
			wx = WX;
			if (doordir == "up")
			{
				WY += 0.1;
			}
			else if (doordir == "down")
			{
				WY -= 0.1;
			}
			if (doordir == "right")
			{
				WX += 0.1;
			}
			else if (doordir == "left")
			{
				WX -= 0.1;
			}
			if (doordir == "forward")
			{
				WZ += 0.1;
			}
			else if (doordir == "backward")
			{
				WZ -= 0.1;
			}
		} 
		else
		{
			WX = wx;  
			WY = wy;
			WZ = wz;
		}
		glTranslatef(WX, WY, WZ); 
	} 
	else
	{
		glTranslatef(wx, wy, wz);
	}
	
	glColor3f(1, 1, 1);
	glScalef(scalex, scaley, scalez);
	if (type == 1)
	{
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex3f(-1, -1, 0);
		glTexCoord2i(0, 0); glVertex3f(-1, 1, 0);
		glTexCoord2i(1, 0); glVertex3f(1, 1, 0);
		glTexCoord2i(1, 1); glVertex3f(1, -1, 0);
		glEnd();

		if (cubecollide(px, py, pz, wx - scalex, wx + scalex, wy - scaley, wy + scaley, wz - 0.7, wz + 0.7))
		{
			std::cout << "WALL HERE\n";
			
				px = oldpx; 
				pz = oldpz;	
		}
	}
	if (type == 2)
	{
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex3f(0, -1, -1);
		glTexCoord2i(0, 0); glVertex3f(0, 1, -1);
		glTexCoord2i(1, 0); glVertex3f(0, 1, 1);
		glTexCoord2i(1, 1); glVertex3f(0, -1, 1);
		glEnd();

		if (cubecollide(px, py, pz, wx - (scalex / 5), wx + (scalex / 5), wy - scaley, wy + scaley, wz - scalez, wz + scalez))
		{
			std::cout << "WALL HERE\n";
			 
				px = oldpx;
				pz = oldpz;	
		}
	}   
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void temple(float tx,float ty,float tz,float Tscale,int tex)
{
	float x = tx;
	float y = ty;
	float z = tz; 
	float scalex = Tscale; 
	float scaley = Tscale;
	float scalez = Tscale;
	Floor(x, y, z, scalex/20,scaley/20, scalez/20, texs[tex]);
	WallDraw(x - scalex, y + scaley, z, 2, scalex, scaley, scalez, tex,"");
	WallDraw(x + scalex, y + scaley, z, 2, scalex, scaley, scalez, tex,"");
	Floor(x, y + (scaley * 2), z, scalex/20, scaley/20,scalez/20, texs[tex]);
	WallDraw(x, y + scaley, z - scalez, 1, scalex, scaley, scalez, 11,"left");
	WallDraw(x, y + scaley, z + scalez, 1, scalex, scaley, scalez, tex,"");
}

void skybox()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	float x = 0, y = 0, z = 0; // Position at camera
	float size = 10;
	glPushMatrix();
	glTranslatef(x, y, z);
	glColor4f(1, 1, 1, 1);
	// Right
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
	glTexCoord2f(1, 1); glVertex3f(size, size, -size);
	glTexCoord2f(0, 1); glVertex3f(size, size, size);
	glTexCoord2f(0, 0); glVertex3f(size, -size, size);
	glEnd();

	// Left
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(-size, -size, size);
	glTexCoord2f(1, 1); glVertex3f(-size, size, size);
	glTexCoord2f(0, 1); glVertex3f(-size, size, -size);
	glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
	glEnd();

	// Top
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-size, size, -size);
	glTexCoord2f(0, 0); glVertex3f(-size, size, size);
	glTexCoord2f(1, 0); glVertex3f(size, size, size);
	glTexCoord2f(1, 1); glVertex3f(size, size, -size);
	glEnd();

	// Bottom
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-size, -size, -size);
	glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
	glTexCoord2f(0, 0); glVertex3f(size, -size, size);
	glTexCoord2f(0, 1); glVertex3f(-size, -size, size);
	glEnd();

	// Front
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-size, -size, size);
	glTexCoord2f(1, 0); glVertex3f(size, -size, size);
	glTexCoord2f(1, 1); glVertex3f(size, size, size);
	glTexCoord2f(0, 1); glVertex3f(-size, size, size);
	glEnd();

	// Back
	glBindTexture(GL_TEXTURE_2D, texs[16]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
	glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
	glTexCoord2f(0, 1); glVertex3f(-size, size, -size);
	glTexCoord2f(1, 1); glVertex3f(size, size, -size);
	glEnd();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

}

void terrainGen()
{
	for (auto& t : tilez)
	{
		float dx = px - 0;
		float dy = py - 0;
		float dz = pz - 0;
		float dist = sqrt(dx * dx + dy * dy + dz * dz);
		if (dist> 37)
		{
			static float timer = 0; 
			timer += 0.3;
			if (timer > 1)
			{
				for (int i = 0; i < 3; i++)
					tilez.push_back({ px + i * 3,0,pz + i * 3,3,1,3,texs[17] }); 
				timer = 0;
			}
				
		}
	}
}

void animatedSprite(glm::vec3 pos,glm::vec3 scale,int frames,int id)
{
	glPushMatrix(); 
	static int tex=1;
	glEnable(GL_TEXTURE_2D);
	switch (id)
	{
	case(0): 
	{
		glBindTexture(GL_TEXTURE_2D, animatedGuyTexs[tex]);
	}
	break; 
	case(1): 
	{
		glBindTexture(GL_TEXTURE_2D,aluminiumGuyTexs[tex]);
	} 
	break;
	}

	float dx =pos.x-px;
	float dz = pos.z-pz;
	float Angle = atan2(dz,dx);
	glTranslatef(pos.x,pos.y,pos.z); 
	glRotatef(Angle,0,1,0); 
	glScalef(scale.x, scale.y, scale.z);
	static float timer = 0; 
	timer += 0.1;
	if(tex <frames)
	{
	
		if (timer > tex * 2)
		{
			tex = tex + 1; 
			timer = 0;
		}
			
		if (tex >= frames)
			tex = 1;
	}
	
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(1, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(-1, 1, 0);
	glTexCoord2f(0, 1); glVertex3f(-1, -1, 0);
	glTexCoord2f(1, 1); glVertex3f(1, -1, 0);
	glEnd();

	glPopMatrix();
}

struct NPCharacters
{
	glm::vec3 Pos; 
	glm::vec3 size; 
	glm::vec3 rotation; 
	std::string Character_ID;
	glm::vec3 velocity;
	float timer=0;  
	bool forward, backward, left, right, up, down;
	void ControlTheCharacters()
	{
		velocity *= 0.9;
		Pos += velocity;
		static int cID = 0; 
		if (Character_ID == "aluminiumMan")
			cID = 0;

		switch (cID)
		{
		case(0): 
		{ 
			timer += 0.1; 
			if (timer > 2)
			{
				velocity.y += 1;
				timer = 0;
			}

			animatedSprite(Pos,size,4,cID);  
		} 
		break;
	    }
	}
};
std::vector<NPCharacters>npcz;
void boingGuy()
{   
	static float bgx=0, bgy=7, bgz=0; 
	static float Incr=0;
	Incr += 0.1; 
	bgy = 7+sin(Incr)*5;
	float dx = bgx-px; 
	float dz = bgz-pz; 
	float dy = bgy-py;
	float dist = sqrt(dx * dx + dy * dy + dz * dz);	
	float bgA = atan2(dx, dz) * (180 / 3.14159);
	static bool left=false, right=true;
	static float timer = 0;
	timer += 0.1; 
	if (timer < 37)
	{
		bgx += 0.1; 
		right = true;
		left = true;
	}
	if (timer > 37)
	{
		right = false;
		left = true; 
		bgx -= 0.1;
	}
	if (timer > 74)
	{
		right = false;
		left = false;
		timer = 0;
	}
	
	if (cubecollide(px,py,pz,bgx-2,bgx+2,bgy-4,bgy+4,bgz-4,bgz+4))
	{
		px -= dx / dist; 
		py -= dy / dist;
		pz -= dz / dist; 
		bgx += dx / dist;
		bgy += dy / dist;
		bgz += dz / dist;
		
	} 
	for (auto& w : wallz)
	{
		float DX = w.WX - bgx;
		float DY = w.WY - bgy;
		float DZ = w.WZ - bgz;
		float Dist = sqrt(DX*DX+DY*DY+DZ*DZ); 
		if (cubecollide(bgx, bgy, bgz, w.WX-5,w.WX+5, w.WY-3,w.WY+3, w.WZ-5,w.WZ+5))
		{
			bgx += DX / Dist;
			bgy += DY / Dist;
			bgz += DZ / Dist; 
			if(right==true)
				bgx -= 7;  
			if(left==true)
				bgx += 7;
		}  
	}

	OBJdrawer(coolBoy, bgx, bgy, bgz, bgA, 1, "orange", 1);
}

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	glLoadIdentity();     
	skybox();
	Player();
	//terrainGen();
	for (auto& W : wallz)
		W.WallDraw();
	for (auto& T : tilez)
		T.Floor(); 
	for (auto& t : toolz)
		t.toolMangager(); 
	for (auto& n : npcz)
		n.ControlTheCharacters();

	temple(-37,0,0,12,7);
	Floor(0,0,0,1,0,1,texs[17]);
	animatedSprite(glm::vec3(-37,7, 0), glm::vec3(7, 7, 7), 4, 0);
	animatedSprite(glm::vec3(12,7,0),glm::vec3(7,7,7),4,1);
	boingGuy(); 
	glutSwapBuffers();
}

void reshaper(int w,int h)
{
	glViewport(0,0,w,h); 
	glMatrixMode(GL_PROJECTION);  
	gluPerspective(70, 16.0 / 9.0, 0.1, 10000); 
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity();
}

void mouseClickThing(int button,int state,int mx,int my)
{   
	if (tool==1)
	{
		float PlayerDirX = px + cos((pry + 90) * rads);
		float PlayerDirZ = pz - sin((pry + 90) * rads);
		float wallPlaceX;
		float wallPlaceZ;
		float wallPlaceY = floor(round(py / 3) * 3);
		if (button == GLUT_LEFT)
		{
			wallPlaceX = floor(round(PlayerDirX / 3) * 3);
			wallPlaceZ = floor(round(PlayerDirZ / 3) * 3);
			wallz.push_back({ wallPlaceX,wallPlaceY,wallPlaceZ,wallTex,"X",3,3,1 });
			Sleep(100);
		}
		if (button == GLUT_RIGHT_BUTTON)
		{
			wallPlaceZ = floor(round(PlayerDirZ / 3) * 3);
			wallPlaceX = floor(round(PlayerDirX / 3) * 3);
			wallz.push_back({ wallPlaceX,wallPlaceY,wallPlaceZ,wallTex,"Z",1,3,3 });
			Sleep(100);
		}
		if (button == GLUT_MIDDLE_BUTTON)
		{
			wallPlaceZ = floor(round(PlayerDirZ/ 6) * 6);
			wallPlaceX = floor(round(PlayerDirX/ 6) * 6);
			tilez.push_back({ wallPlaceX,wallPlaceY - 3,wallPlaceZ,3,1,3,texs[wallTex] });
			Sleep(100);
		}

	}
	
}

void passiveMotion(int mx,int my)
{
	if (camMove == true)
	{
		int dx = (SW / 2) - mx;
		int dy = (SH / 2) - my;
		pry += (float)dx / 10;
		prx += (float)dy / 10;
	}
}

void loadingOBJS()
{
	OBJloader(coolBoy,"C:/Users/joemama23x/comtex/sinkMan.obj");
	OBJloader(hammer,"C:/Users/joemama23x/comtex/hammer.obj");
}

void CharacterLoader(glm::vec3 pos, glm::vec3 scale,glm::vec3 rotation, std::string ID)
{
	npcz.push_back({pos,scale,rotation,ID});
}

void texloader()
{ 
	
	animatedGuyTexs[1] = texload("C:/Users/joemama23x/comtex/snowstand.png");
	animatedGuyTexs[2] = texload("C:/Users/joemama23x/comtex/snowalk1.png");
	animatedGuyTexs[3] = texload("C:/Users/joemama23x/comtex/snowalk3.png"); 
	animatedGuyTexs[4] = texload("C:/Users/joemama23x/comtex/snowalk4.png");
	aluminiumGuyTexs[1] = texload("C:/Users/joemama23x/comtex/earthshock1.png"); 
	aluminiumGuyTexs[2] = texload("C:/Users/joemama23x/comtex/earthshock2.png"); 
	aluminiumGuyTexs[3] = texload("C:/Users/joemama23x/comtex/earthshock3.png"); 
	aluminiumGuyTexs[0] = texload("C:/Users/joemama23x/comtex/earthshock4.jpg");
	
	texs[1] = texload("C:/Users/joemama23x/comtex/gears2.png");
	texs[2] = texload("C:/Users/joemama23x/comtex/stones.png");
	texs[3] = texload("C:/Users/joemama23x/comtex/swimmingpool.png");
	texs[4] = texload("C:/Users/joemama23x/comtex/whitebricks.png");
	texs[5] = texload("C:/Users/joemama23x/comtex/redtrip2.png");
	texs[6] = texload("C:/Users/joemama23x/comtex/walkertri.png");
	texs[7] = texload("C:/Users/joemama23x/comtex/pyramidbricks.png");
	texs[8] = texload("C:/Users/joemama23x/comtex/triart1.png");
	texs[9] = texload("C:/Users/joemama23x/comtex/theMartians.png");
	texs[10] = texload("C:/Users/joemama23x/comtex/coloredrects.png");
	texs[11] = texload("C:/Users/joemama23x/comtex/steeldoor.png");
	texs[12] = texload("C:/Users/joemama23x/comtex/Abutton.png");
	texs[14] = texload("C:/Users/joemama23x/comtex/martianart.png");
	texs[15] = texload("C:/Users/joemama23x/comtex/AIMER37.png");
	texs[16] = texload("C:/Users/joemama23x/comtex/greysky.png");
	texs[17] = texload("C:/Users/joemama23x/comtex/grass.png");
	texs[18] = texload("C:/Users/joemama23x/comtex/leafy_grass.png");
	texs[19] = texload("C:/Users/joemama23x/comtex/brickwall.png");
	texs[20] = texload("C:/Users/joemama23x/comtex/zigger.png");
}

void Initialize()
{
	GLfloat pos[] = { px,py + 25,pz,1 };
	py = 3;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambeint);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
	glEnable(GL_NORMALIZE);
	loadingOBJS();
	wallTex = 1;
	toolz.push_back({ 70,7,37,1,hammer });
	texloader(); 
	CharacterLoader(glm::vec3(7,0,0),glm::vec3(1,1,1),glm::vec3(0,0,0),"aluminiumMan");
}
 
void Time(int clock)
{ 
	clock = 10; 
	glutPostRedisplay();
	if(camMove==true)
	glutWarpPointer(SW / 2, SH / 2);
	GLfloat pos[] = { px,py + 25,pz,1 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glutTimerFunc(clock,Time,0); 
	UniTime += 1;   
	vy *= 0.8; 
	if (onobject == true)
	{
		gravity = false; 
		vy = 0;
	}  
	if (!onobject == true)
	{
		gravity = true;
	} 
	gravity = true; 
	if (gravity == true)
	{ 
		onobject = false;  
		py += vy;
		vy = -0.1;  
		if (vy < -0.1)
			vy = -0.1; 
	}   
}

int main(int argc, char* args[])
{    
	glutInit(&argc, args);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH); 
	glutInitWindowPosition(100,100); 
	glutInitWindowSize(SW,SH);  
	glutCreateWindow("rizzler");   
	glewInit();
	Initialize(); 
	glutTimerFunc(0,Time,0);  
	glutSpecialFunc(special); 
	glutSpecialUpFunc(notsospecial);
	glutKeyboardFunc(keys1); 
	glutKeyboardUpFunc(keys2); 
	glutReshapeFunc(reshaper);  
	glutMouseFunc(mouseClickThing); 
	glutPassiveMotionFunc(passiveMotion);
	glutDisplayFunc(display); 
	glutMainLoop();    
	return 0;
}