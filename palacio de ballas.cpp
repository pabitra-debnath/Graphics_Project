#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
#include<GL/glut.h>


double cameraAngle;
double move_X, move_Y, move_Z;
int canDrawGrid, canDrawAxis;
double speed=30,ang_speed=.1;
double cameraRadius, cameraHeight, cameraAngleDelta;
int num_texture  = -1;
GLuint skyimage,surfaceimg,tximg,rightlowimg,front1img,frontimg,floorimg,florimg,walltximg,
	pilerimg,frontbigwinimg,uppilerimg,upsidewinimg,drawimg,sidedoorimg,statueimg,updesimg,
	doorupimg,upbelimg,up1img,roofimg,sidewinnimg,sidewallimg,sideimg,backimg,roofwinimg,
	wallimg,domeimg,roofsideimg,mbesidecyimg,pbesidecyimg,roofstatueimg,ftwallimg,doorimg;

/***************************** VECTOR structure **********************************/

struct V;

V operator+(V a,V b);
V operator*(V a,V b);
V operator*(V b,double a);
V operator*(double a,V b);

struct V{
	double x,y,z;

	V(){}
	V(double _x,double _y){x=_x;y=_y;z=0;}
	V(double _x,double _y,double _z){x=_x;y=_y;z=_z;}

	
	double	mag(){	return sqrt(x*x+y*y+z*z);	}
	
	void 	norm(){	double d = mag();x/=d;y/=d;	z/=d;}
	V 		unit(){	V ret = *this;	ret.norm(); return ret;}

	double	dot(V b){		return x*b.x + y*b.y + z*b.z;}
	V		cross(V b){		return V( y*b.z - z*b.y , z*b.x - x*b.z , x*b.y - y*b.x );}
	

	double	projL(V on){	on.norm();	return this->dot(on);}
	V		projV(V on){	on.norm();	return on * projL(on);}

	V rot(V axis, double angle){
		return this->rot(axis, cos(angle), sin(angle));
	}
	
	V rot(V axis, double ca, double sa){
		V rotatee = *this;
		axis.norm();
		V normal = (axis * rotatee).unit();
		V mid = (normal * axis).unit();
		double r = rotatee.projL(mid);
		V ret=r*mid*ca + r*normal*sa + rotatee.projV(axis);
		return ret.unit();
	}
};

V operator+(V a,V b){		return V(a.x+b.x, a.y+b.y, a.z+b.z);	}
V operator-(V a){			return V (-a.x, -a.y, -a.z);			}
V operator-(V a,V b){		return V(a.x-b.x, a.y-b.y, a.z-b.z);	}
V operator*(V a,V b){		return a.cross(b);						}
V operator*(double a,V b){	return V(a*b.x, a*b.y, a*b.z);			}
V operator*(V b,double a){	return V(a*b.x, a*b.y, a*b.z);			}
V operator/(V b,double a){	return V(b.x/a, b.y/a, b.z/a);			}




V loc,dir,perp;


V  _L(0,-150,20);
V  _D(0,1,0);
V  _P(0,0,1);

/***************************** Texture Functions *******************************/

int LoadBitmapImage(char *filename)
{
    int i, j=0;
    FILE *l_file;
    unsigned char *l_texture;

    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    num_texture++;

    if( (l_file = fopen(filename, "rb"))==NULL) return (-1);

    fread(&fileheader, sizeof(fileheader), 1, l_file);

    fseek(l_file, sizeof(fileheader), SEEK_SET);
    fread(&infoheader, sizeof(infoheader), 1, l_file);

    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
	for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
		{
				fread(&rgb, sizeof(rgb), 1, l_file);

				l_texture[j+0] = rgb.rgbtRed;
				l_texture[j+1] = rgb.rgbtGreen;
				l_texture[j+2] = rgb.rgbtBlue;
				l_texture[j+3] = 255;
				j += 4;
		}
    fclose(l_file);

    glBindTexture(GL_TEXTURE_2D, num_texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture);

    return (num_texture);

}

void loadImage()
{
	doorimg = LoadBitmapImage("image/door.bmp");
	ftwallimg = LoadBitmapImage("image/ftwall.bmp");
	roofstatueimg = LoadBitmapImage("image/roofstatue.bmp");
	mbesidecyimg = LoadBitmapImage("image/mbesidecy.bmp");
	pbesidecyimg = LoadBitmapImage("image/pbesidecy.bmp");
	domeimg = LoadBitmapImage("image/dome.bmp");
	roofsideimg = LoadBitmapImage("image/roofside.bmp");
	roofwinimg = LoadBitmapImage("image/roofwin.bmp");
	backimg = LoadBitmapImage("image/back.bmp");
	sideimg = LoadBitmapImage("image/side.bmp");
	sidewallimg = LoadBitmapImage("image/sidewall.bmp");
	sidewinnimg = LoadBitmapImage("image/sidewinn.bmp");
	roofimg = LoadBitmapImage("image/rooftop.bmp");
	up1img = LoadBitmapImage("image/up1.bmp");
	upbelimg = LoadBitmapImage("image/upbel.bmp");
	doorupimg = LoadBitmapImage("image/doorup.bmp");
	rightlowimg = LoadBitmapImage("image/rightlow.bmp");
	updesimg = LoadBitmapImage("image/updes.bmp");
	statueimg = LoadBitmapImage("image/statue.bmp");
	sidedoorimg = LoadBitmapImage("image/sidedoor.bmp");
	drawimg = LoadBitmapImage("image/draw.bmp");
	upsidewinimg = LoadBitmapImage("image/upsidewin.bmp");
	frontbigwinimg = LoadBitmapImage("image/frontbigwindow.bmp");
	walltximg = LoadBitmapImage("image/walltx.bmp");
	wallimg = LoadBitmapImage("image/wall.bmp");
	florimg = LoadBitmapImage("image/flor.bmp");
	floorimg = LoadBitmapImage("image/floor.bmp");
	uppilerimg = LoadBitmapImage("image/uppiler.bmp");
	pilerimg = LoadBitmapImage("image/piler.bmp");
	front1img = LoadBitmapImage("image/front1.bmp");
	frontimg = LoadBitmapImage("image/front.bmp");
	//surfaceimg = LoadBitmapImage("image/surface.bmp");
	//skyimage = LoadBitmapImage("image/sky.bmp");
	if(walltximg != -1)
		printf("Load successful!!\n");
	else printf("Image loading Failed!!\n");
}

/****************************** GridLines and Axes ***********************************/

void drawGridAndAxes(){
	
	// draw the three major AXES
	
	glBegin(GL_LINES);
		//X axis
		glColor3f(0, 1, 0);	//100% Green
		glVertex3f(-150, 0, 0);
		glVertex3f( 150, 0, 0);
		
		//Y axis
		glColor3f(0, 0, 1);	//100% Blue
		glVertex3f(0, -150, 0);	// intentionally extended to -150 to 150, no big deal
		glVertex3f(0,  150, 0);
		
		//Z axis
		glColor3f(1, 1, 1);	//100% White
		glVertex3f( 0, 0, -150);
		glVertex3f(0, 0, 150);
	glEnd();
	
	//some gridlines along the field
	int i;

	glColor3f(0.5, 0.5, 0.5);	//grey
	glBegin(GL_LINES);
		for(i=-10;i<=10;i++){

			if(i==0)
				continue;	//SKIP the MAIN axes

			//lines parallel to Y-axis
			glVertex3f(i*10, -100, 0);
			glVertex3f(i*10,  100, 0);

			//lines parallel to X-axis
			glVertex3f(-100, i*10, 0);
			glVertex3f( 100, i*10, 0);
		}
	glEnd();
	
}
/*********************************** Make Custom Cube ***********************************/
void solidCube1Part(double size) {
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-size, 0, size);
	glTexCoord2f(0, 0);
	glVertex3f(size, 0, size);
	glTexCoord2f(1, 0);
	glVertex3f(size, 0, -size);
	glTexCoord2f(1, 1);
	glVertex3f(-size, 0, -size);
	glEnd();
}

// up and down part
void solidCube2Part(double size) {
	solidCube1Part(size);
	glRotated(90, 1, 0, 0);
	glTranslated(0, -size, -size);
	solidCube1Part(size);
}

void customSolidCube(double size) {
	glPushMatrix(); {
		glTranslated(0, -size/2, 0);
		solidCube2Part(size/2);
		glRotated(90, 1, 0, 0);
		glTranslated(0, -size/2, -size/2);
		solidCube2Part(size/2);
	}glPopMatrix();

	glPushMatrix(); {
		glRotated(90, 0, 0, 1);
		glTranslated(0, size/2, 0);
		solidCube1Part(size/2);
	}glPopMatrix();

	glPushMatrix(); {
		glRotated(90, 0, 0, 1);
		glTranslated(0, -size/2, 0);
		solidCube1Part(size/2);
	}glPopMatrix();
}
/******************************** /End Make Custom Cube *********************************/
void sidewin(){
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,1,35);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, upsidewinimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();

}
void roof(){
	glPushMatrix();{
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(1,1,1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();
}
void belpiler(){
	glPushMatrix();
	glTranslatef(0,0,0);
	//glScalef(30,3,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, up1img);
		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);
		gluCylinder(cylindar,3,3,35,20,20);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void piler(){
	glPushMatrix();
	glTranslatef(0,0,0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pilerimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,3,3,37,20,20);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();{
	glColor3f(.8,1,.8);
	glTranslatef(0,0,1);
	glutSolidTorus(2, 2, 25, 25);
	}glPopMatrix();

	glPushMatrix();{
	glColor3f(.8,1,.8);
	glTranslatef(0,0,39);
	glutSolidTorus(2, 2, 25, 25);
	}glPopMatrix();

}
void uppiler(){
	glPushMatrix();
	glTranslatef(0,0,0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uppilerimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,3.5,3.5,40,20,20);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();{
	glColor3f(0,.6,.6);
	glTranslatef(0,0,2);
	glutSolidTorus(2.3, 2.3 , 25, 25);
	}glPopMatrix();

	glPushMatrix();{
	glColor3f(0,.6,.6);
	glTranslatef(0,0,38);
	glutSolidTorus(2.3, 2.3, 25, 25);
	}glPopMatrix();

}
void door(){

	glPushMatrix();{
	glTranslatef(-28,0,20);
	glRotatef(30,0,0,1);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(50,4,20);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pilerimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(28,0,20);
	glRotatef(-30,0,0,1);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(50,4,20);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pilerimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(0,5,20);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(35,4,40);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, doorimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	
}
void front(){

	glPushMatrix();{
		glTranslatef(0,6,5);
		glScalef(1.35,1,1.2);
		door();
	}glPopMatrix();

glPushMatrix();{

	glPushMatrix();{
	glTranslatef(-40,0,10);
	piler();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(40,0,10);
	piler();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(-32,-23,10);
	piler();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(32,-23,10);
	piler();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(-15,-37,10);
	piler();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(15,-37,10);
	piler();
	}glPopMatrix();


	//.....
	glPushMatrix();{
	glTranslatef(0,0,0);
	glPushMatrix();{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,50,50,5,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	//...floor...
	glPushMatrix();{
	glRotatef(0,0,0,1);
	glPushMatrix();{
	glTranslatef(0,0,5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,50,0,.1,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();
	//....
	

	

	glPushMatrix();{
	glRotatef(0,0,0,1);
	glPushMatrix();{
	glTranslatef(0,0,48);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, frontimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,45,45,15,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	//..floor...
	glPushMatrix();{
	glTranslatef(0,0,10);
	glPushMatrix();{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,45,0,.1,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(0,0,5);
	glPushMatrix();{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,45,45,5,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glRotatef(0,0,0,1);
	glPushMatrix();{
	glTranslatef(0,0,50);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,45,0,.1,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glRotatef(0,0,0,1);
	glPushMatrix();{
	glTranslatef(0,0,60);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, florimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,45,0,.1,20,20);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glColor3f(.8,1,.8);
	glTranslatef(0,0,5.5);
	glutSolidTorus(.8, 45, 25, 25);
	}glPopMatrix();

}glPopMatrix();
	
}
void upperfront(){
	
	glPushMatrix();{
	glTranslatef(-32,20,80);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,1,20);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, uppilerimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(32,20,80);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,1,20);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, uppilerimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();



	glPushMatrix();{
		glTranslatef(0,20,107);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(18,1,35);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, drawimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();
	glTranslatef(-17.5,20,60);
	glPushMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, frontbigwinimg);

		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);
			glVertex3f(0,0,40);
			glTexCoord2f(0,0);
			glVertex3f(0,0,0);
			glTexCoord2f(1,0);
			glVertex3f(35,0,0);
			glTexCoord2f(1,1);
			glVertex3f(35,0,40);
		glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();


	glPushMatrix();{
		glTranslatef(21,19,60);
		uppiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-21,19,60);
		uppiler();
	}glPopMatrix();

	//..torus...
	glPushMatrix();{
	
			glTranslatef(0,18,98);
			glPushMatrix();
			{
				glColor3f(.5,.5,.5);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);
				glEnable(GL_CLIP_PLANE0);{
					glutSolidTorus(3.5,20,25,25);
				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();


	
	glPushMatrix();{
		glTranslatef(26,10,60);
		uppiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-26,10,60);
		uppiler();
	}glPopMatrix();


	glPushMatrix();{
	
			glTranslatef(0,10,98);
			glPushMatrix();
			{
				glColor3f(.57,.57,.57);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);
				glEnable(GL_CLIP_PLANE0);{
					glutSolidTorus(3.5,25,25,25);
				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();

	glPushMatrix();{
		glTranslatef(32,2.5,60);
		glScalef(1.4,1.4,1);
		uppiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-32,2.5,60);
		glScalef(1.4,1.4,1);
		uppiler();
	}glPopMatrix();


	glPushMatrix();{
	
			glTranslatef(0,2,98);
			glPushMatrix();
			{
				glColor3f(.5,.5,.5);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);
				glEnable(GL_CLIP_PLANE0);{
					glutSolidTorus(4,31,25,25);
				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();
	
			//..roof torus......//
		glPushMatrix();{
	
			glTranslatef(-0,50,98);
			glPushMatrix();
			{
				glColor3f(.5,.5,.5);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);

				glEnable(GL_CLIP_PLANE0);{
					glPushMatrix();
					glTranslatef(0,0,0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, floorimg);

					glColor3f(1,1,1);
					GLUquadricObj *cylindar = gluNewQuadric();
					gluQuadricTexture(cylindar, GL_TRUE);

					gluCylinder(cylindar,34,34,50,25,25);

			glDisable(GL_TEXTURE_2D);
			glPopMatrix();	

				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();
}
void sidefront(){
		glPushMatrix();{
		glTranslatef(42,2,1);
		glScalef(1,1,1);
		piler();
	}glPopMatrix();

		glPushMatrix();{
		glTranslatef(70,2,1);
		glScalef(1,1,1);
		piler();
	}glPopMatrix();


	glPushMatrix();{
	glTranslatef(56,-3,44);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(12,4,40);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, front1img);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	//.... statue........
	glPushMatrix();{
		glTranslatef(57,3,20);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,4,25);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, statueimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	//....1st floor right...
		glPushMatrix();{
			glColor3f(1,1,1);
			glTranslatef(53,10,45);
			glScalef(45,25,10);
			//glScalef(80,25,12);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, florimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();

		glPushMatrix();{
		glTranslatef(56,0,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(15,4,48);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, updesimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
		}glPopMatrix();

	glPushMatrix();{
		glTranslatef(40,2,50);
		glScalef(.8,.8,.9);
		piler();
	}glPopMatrix();

		glPushMatrix();{
		glTranslatef(72,2,50);
		glScalef(.8,.8,.9);
		piler();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(58,15,67);
		glScalef(1.1,1,1);
		sidewin();
	}glPopMatrix();

	//..roof...
	glPushMatrix();{
	glTranslatef(55,10,100);
	glScalef(53,20,1);
	roof();
	}glPopMatrix();
		
}
void rightend(){
	
	glPushMatrix();{
		glTranslatef(90,-8,20);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,1,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rightlowimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(90,-10,44);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(13,2,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, doorupimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(90,3,44);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(13,24,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, front1img);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(76,3,45);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(90,22,2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, walltximg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(104,3,45);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(90,22,2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, walltximg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();


	//....1st floor right...
		glPushMatrix();{
			glColor3f(1,1,1);
			glTranslatef(90,8,46);
			glScalef(28,30,10);
			//glScalef(80,25,12);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, florimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();


	glPushMatrix();{
		glTranslatef(76,-6.5,50);
		glScalef(.6,.6,.85);
		piler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(104,-6.5,50);
		glScalef(.6,.6,.85);
		piler();
	}glPopMatrix();


	glPushMatrix();{
		//glTranslatef(56,0,92);
		glTranslatef(76,2,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(15,20,4);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, updesimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
		}glPopMatrix();

	glPushMatrix();{
		//glTranslatef(56,0,92);
		glTranslatef(104,2,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(15,20,4);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, updesimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
		}glPopMatrix();

	glPushMatrix();{
		glTranslatef(91,15,67);
		glScalef(.8,1,1);
		sidewin();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(90,-7,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			//glScalef(15,4,48);
			glScalef(15,3,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, upbelimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
	}glPopMatrix();
	
	glPushMatrix();{
		glTranslatef(82,-6,50);
		belpiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(99,-6,50);
		belpiler();
	}glPopMatrix();

	//..roof...
	glPushMatrix();{
	glTranslatef(90,12,100);
	glScalef(32,30,1);
	roof();
	}glPopMatrix();
}
void main_front(){
	////.....front Start.......//////
	glPushMatrix();{
		glTranslatef(0,0,-13);
	upperfront();
	}glPopMatrix();

	glPushMatrix();{
		glScalef(.8,.8,.8);
		front();
	}glPopMatrix();
	
	glPushMatrix();{
		sidefront();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-112,0,0);
		sidefront();
	}glPopMatrix();

	
	glPushMatrix();{
		rightend();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-180,0,0);
		rightend();
	}glPopMatrix();

////.....front end.......//////
}
void belcony(){
	glPushMatrix();{
	glTranslatef(22,0,0);
	glPushMatrix();{

	glPushMatrix();{
		glTranslatef(0,0,20);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(40,1,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sidewinnimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,1.5,45);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(11,4,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, front1img);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	//....1st floor right...
		glPushMatrix();{
			glColor3f(1,1,1);
			glTranslatef(0,11,47);
			glScalef(50,20,2);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, florimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
	
		glPushMatrix();{
		glTranslatef(11,1.5,51);
		glScalef(.8,.7,.85);
		piler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-11,1.5,51);
		glScalef(.8,.7,.85);
		piler();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(0,0,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			//glScalef(15,4,48);
			glScalef(15,3,32);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, upbelimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,18,70);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(45,1,60);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	}glPopMatrix();
	}glPopMatrix();


}
void swall(){
	glPushMatrix();{
		glTranslatef(3,2,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,4,6);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sidewallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(134,2,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,4,6);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sidewallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();
	
}
void sidebelcony(){
	{
	int t=0;
	for(int i=0;i<5;i++){
		
		belcony();
		glTranslatef(t,0,0);
		t=31.5;
	}
	}
	
	glPushMatrix();{
		glTranslatef(-124,-2,0);
		swall();
	}glPopMatrix();
}
void sidend(){
	glPushMatrix();{
		glTranslatef(50,-20.5,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,1,100);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sideimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(3,-10,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,20,6);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sidewallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(97,-10,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,20,6);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, sidewallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	//..roof...
	glPushMatrix();{
	glTranslatef(50,-10,100);
	glScalef(100,22,1);
	roof();
	}glPopMatrix();
}
void backpiler(){

	glPushMatrix();{
	glTranslatef(0,0,0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sideimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,15,15,100,10,10);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();

	glPushMatrix();{
	glTranslatef(0,0,100);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, roofimg);

		glColor3f(1,1,1);
		GLUquadricObj *cylindar = gluNewQuadric();
		gluQuadricTexture(cylindar, GL_TRUE);

		gluCylinder(cylindar,15,.1,1,10,10);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();

}
void back(){
	glPushMatrix();{
		glTranslatef(0,0,50);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(100,1,100);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, backimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

}
void sidefrontw(){
	glPushMatrix();{
		glTranslatef(0,0,45);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(90,30,20);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, walltximg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,0,44);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(13,32,22);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, front1img);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,0,92);
		glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			//glScalef(15,4,48);
			glScalef(15,32,22);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, updesimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}
		glPopMatrix();
	}glPopMatrix();
}
void sidefrontr(){
	glPushMatrix();{
		glTranslatef(0,0,-13);
	upperfront();
	}glPopMatrix();

	glPushMatrix();{
		glScalef(.8,.8,.8);
		front();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(42,18,0);
		glScalef(.8,1.2,1);
		sidefrontw();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(-42,18,0);
		glScalef(.8,1.2,1);
		sidefrontw();
	}glPopMatrix();

	
	

}
void right(){
	/////...Right side..../////

	glPushMatrix();{
		glTranslatef(115,-63,0);
		glPushMatrix();{
		glRotatef(90,0,0,1);
		rightend();
		}glPopMatrix();
	}glPopMatrix();
	
	
	glPushMatrix();{
		glTranslatef(145,92,0);
		glRotatef(90,0,0,1);
		sidefrontr();
	}glPopMatrix();


	glPushMatrix();{
		glTranslatef(125,142,0);
		glRotatef(90,0,0,1);
		sidebelcony();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(125,275,0);
		glRotatef(90,0,0,1);
		sidend();
	}glPopMatrix();
}

void left(){
	//...left side..../////

	glPushMatrix();{
	glTranslatef(-115,-63,0);
	glPushMatrix();{
		glRotatef(-90,0,0,1);
		glPushMatrix();{
			glTranslatef(-180,0,0);
			rightend();
		}glPopMatrix();
	}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-145,92,0);
		glRotatef(-90,0,0,1);
		sidefrontr();
	}glPopMatrix();

	glPushMatrix();{
		//glTranslatef(-127,275,0);
		glTranslatef(-125,275,0);
		glRotatef(-90,0,0,1);
		sidebelcony();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-125,375,0);
		glRotatef(-90,0,0,1);
		sidend();
	}glPopMatrix();


}
void backside(){

	
	glPushMatrix();{
		glTranslatef(138,180,0);
		backpiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-138,180,0);
		backpiler();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-0,185,0);
		glScalef(2.7,1,1);
		back();
	}glPopMatrix();
}
void dome(){
		glPushMatrix();{
	glTranslatef(0,-120,135);
	glScalef(2.1,1.5,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, domeimg);

		glColor3f(1,1,1);
		GLUquadricObj *sphere = gluNewQuadric();
		gluQuadricTexture(sphere, GL_TRUE);

		gluSphere(sphere, 20, 10, 10);

	glDisable(GL_TEXTURE_2D);
	}glPopMatrix();

}
void upright(){
	glPushMatrix();{
		glTranslatef(70,-139,108);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(15,45,50);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofsideimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(70,-89,108);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(15,55,50);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofsideimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(70,-112,116);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(1,100,50);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, wallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

}
void roofup(){
	
	glPushMatrix();{
		glPushMatrix();{
			glTranslatef(50,30,-23);
			glScalef(1,1.2,1);
			dome();
		}glPopMatrix();

		upright();
	}glPopMatrix();

	glPushMatrix();{
		glPushMatrix();{
			glTranslatef(-50,30,-23);
			glScalef(1,1.2,1);
			dome();
		}glPopMatrix();

		upright();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-140,0,0);
		upright();
	}glPopMatrix();
	
	glPushMatrix();{

		dome();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,-160,118);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(35,1,90);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofwinimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();
	


	glPushMatrix();{
		glTranslatef(0,-70,118);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(35,1,90);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofwinimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,-115,117);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(30,89,90);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, wallimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();


}
void besidecy(){
	glPushMatrix();{
		glTranslatef(70,83,117);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(35,25,5);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pbesidecyimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(70,149,117);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(35,25,5);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pbesidecyimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(70,116,112);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(25,42,5);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mbesidecyimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();
	
}
void roofback(){
	glPushMatrix();{
		besidecy();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(-140,0,0);
		besidecy();
	}glPopMatrix();

	glPushMatrix();{
	
			glTranslatef(-0,70,92);
			glScalef(1.2,1,1);
			glPushMatrix();
			{
				glColor3f(.5,.5,.5);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);

				glEnable(GL_CLIP_PLANE0);{
					glPushMatrix();
					glTranslatef(0,0,0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, floorimg);

					glColor3f(1,1,1);
					GLUquadricObj *cylindar = gluNewQuadric();
					gluQuadricTexture(cylindar, GL_TRUE);

					gluCylinder(cylindar,40,40,140,12,12);

			glDisable(GL_TEXTURE_2D);
			glPopMatrix();	

				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();

	glPushMatrix();{
	
			glTranslatef(-0,160,82);
			glScalef(1.2,1,1);
			glPushMatrix();
			{
				glColor3f(.5,.5,.5);
				glTranslatef(0,0,0);
				glRotatef(90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);

				glEnable(GL_CLIP_PLANE0);{
					glPushMatrix();
					glTranslatef(0,0,0);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, wallimg);

					glColor3f(1,1,1);
					GLUquadricObj *cylindar = gluNewQuadric();
					gluQuadricTexture(cylindar, GL_TRUE);
					gluCylinder(cylindar,60,60,90,20,20);

					glPushMatrix();{
						glColor3f(.75,.75,.75);
						glutSolidCone(60,.1,20,20);
					}glPopMatrix();

					glPushMatrix();{
						glColor3f(.75,.75,.75);
						glTranslatef(0,0,90);
						glutSolidCone(60,.1,20,20);
					}glPopMatrix();

			glDisable(GL_TEXTURE_2D);
			glPopMatrix();	

				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}glPopMatrix();

}
void statue(){
	glPushMatrix();{
		glTranslatef(0,0,40);
	glPushMatrix();{
			glColor3f(1,1,1);
			glRotatef(90,0,1,0);
			glScalef(80,3,30);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, roofstatueimg);
			customSolidCube(1);
			glDisable(GL_TEXTURE_2D);
		}glPopMatrix();
	}glPopMatrix();

	glPushMatrix();{
		glColor3f(0.4,0,0.2);
		glTranslatef(1,0,82);
		glScalef(16,3,5);
		glutSolidCube(1);
	}glPopMatrix();

	glPushMatrix();{
		glColor3f(0.4,0,0.2);
		glTranslatef(1,0,85);
		glScalef(1,.2,1);
		glutSolidCone(8,15,20,20);
	}glPopMatrix();

	glPushMatrix();{
		glColor3f(0.4,0,0.2);
		glTranslatef(1,0,100);
		glutSolidSphere(5,20,20);
	}glPopMatrix();

	glPushMatrix();{
			glTranslatef(0,0,130);
			glScalef(1,.5,1.5);
			glPushMatrix();
			{
				glColor3f(0.4,0,0.2);
				glTranslatef(0,0,0);
				glRotatef(-90,1,0,0);
				double equ[4];
				equ[0]=0;
				equ[1]=1;
				equ[2]=0;
				equ[3]=0;
				glClipPlane(GL_CLIP_PLANE0,equ);
				glEnable(GL_CLIP_PLANE0);{
					glScalef(.3,.5,.5);
					glutSolidTorus(8,30,50,50);
				}glDisable(GL_CLIP_PLANE0);

			}glPopMatrix();
		}
		glPopMatrix();
}


void drawTorus(double r = 5, double c = 10,
               int rSeg = 16, int cSeg = 8,
               int texture = 0)
{
  glFrontFace(GL_CW);

  glBindTexture(GL_TEXTURE_2D, backimg);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  const double PI = 3.1415926535897932384626433832795;
  const double TAU = 2 * PI;

  for (int i = 0; i < rSeg; i++) {
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= cSeg; j++) {
      for (int k = 0; k <= 1; k++) {
        double s = (i + k) % rSeg + 0.5;
        double t = j % cSeg;

        double x = (c + r * cos(s * TAU / rSeg)) * cos(t * TAU / cSeg);
        double y = (c + r * cos(s * TAU / rSeg)) * sin(t * TAU / cSeg);
        double z = r * sin(s * TAU / rSeg);

        double u = (i + k) / (float) rSeg;
        double v = t / (float) cSeg;

        glTexCoord2d(u, v);
        glNormal3f(2 * x, 2 * y, 2 * z);
        glVertex3d(2 * x, 2 * y, 2 * z);
      }
    }
    glEnd();
  }

  glFrontFace(GL_CCW);
}
void display(){
	//codes for Models, Camera
	
	//clear the display
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear buffers to preset values

	/***************************
	/ set-up camera (view) here
	****************************/ 
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);		//specify which matrix is the current matrix

	//initialize the matrix
	glLoadIdentity();				//replace the current matrix with the identity matrix [Diagonals have 1, others have 0]

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(0,-150,20,	0,0,0,	0,0,1);
	gluLookAt(cameraRadius*sin(cameraAngle), -cameraRadius*cos(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/**************************************************
	/ Grid and axes Lines(You can remove them if u want)
	***************************************************/
	drawGridAndAxes();

	/****************************
	/ Add your objects from here
	***************************/

	/*glPushMatrix();{
		glTranslatef(0,-195,3);
		glScalef(1.1,1,1);
		doorwall();
	}glPopMatrix();*/


	glPushMatrix();{

		glTranslated(0,-120,150);
		glScalef(.4,1,.3);
		statue();
	}glPopMatrix();


///........//////////	
	roofback();
	roofup();

	glPushMatrix();{
		glTranslatef(0,-200,0);
		main_front();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,-200,0);
		right();
	}glPopMatrix();

	glPushMatrix();{
		glTranslatef(0,-200,0);
		left();
	}glPopMatrix();
	
	
	
	//..roof...
	glPushMatrix();{
	glTranslatef(0,4,100);
	glScalef(250,367,1);
	roof();
	}glPopMatrix();

	//...back side..../////

	backside();


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	//codes for any changes in Models, Camera
	
	//cameraAngle += cameraAngleDelta;	// camera will rotate at 0.002 radians per frame.
	
	//codes for any changes in Models

	//MISSING SOMETHING? -- YES: add the following
	glutPostRedisplay();	//this will call the display AGAIN

}

void init(){
	//codes for initialization
	loadImage();
	
	move_X = 0;
	move_Y = 0;
	move_Z = 0;
	canDrawGrid = 1;
	canDrawAxis = 1;

	cameraAngleDelta = .001;


	cameraAngle = 0;	//angle in radian
	cameraRadius = 150;
	cameraHeight = 50;

	//clear the screen
	glClearColor(0,0,0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	
	//initialize the matrix
	glLoadIdentity();

	/*
		gluPerspective() — set up a perspective projection matrix

		fovy -         Specifies the field of view angle, in degrees, in the y direction.
        aspect ratio - Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
        zNear -        Specifies the distance from the viewer to the near clipping plane (always positive).
        zFar  -        Specifies the distance from the viewer to the far clipping plane (always positive).
    */
	
	gluPerspective(70,	1,	0.1,	10000.0);
	
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			move_X += 1;
			break;

		case '2':	
			move_X -= 1;
			break;
			
		case '3':	
			move_Y += 1;
			break;
		case '4':	
			move_Y -= 1;
			break;
			
		case '5':	
			move_Z += 1;
			break;
		case '6':	
			move_Z -= 1;
			break;
		case '8':	

			break;

		case 'g':
			canDrawGrid ^= 1;
			break;
			
		case 'h':
			canDrawAxis ^= 1;
			break;

		case 'p':
			break;

		case 'r':
			break;

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraRadius += 10;
			break;
		case GLUT_KEY_UP:		// up arrow key
			if(cameraRadius > 10)
				cameraRadius -= 10;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.01;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.01;
			break;

		case GLUT_KEY_PAGE_UP:
			cameraHeight += 10;
			break;
		case GLUT_KEY_PAGE_DOWN:
			cameraHeight -= 10;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			cameraAngle = 0;	
			cameraRadius = 150;
			cameraHeight = 50;
			break;
		case GLUT_KEY_END:
		
			break;

		default:
			break;
	}
}

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				cameraAngleDelta = -cameraAngleDelta;	
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

int main(int argc, char **argv){
	
	glutInit(&argc,argv);							//initialize the GLUT library
	
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	
	/*
		glutInitDisplayMode - inits display mode
		GLUT_DOUBLE - allows for display on the double buffer window
		GLUT_RGBA - shows color (Red, green, blue) and an alpha
		GLUT_DEPTH - allows for depth buffer
	*/
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	

	glutCreateWindow("Some Title");

	printf("Camera Control\n");
	printf("_____________\n");
	printf("Zoom In-Out: UP and DOWN arrow\n");
	printf("Camera Rotate: LEFT and RIGHT arrow\n");
	printf("Up-Down: PAGEUP and PAGEDOWN\n");
	printf("Reset Camera: HOME\n");

	init();						//codes for initialization

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
