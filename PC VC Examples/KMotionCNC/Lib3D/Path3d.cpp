//********************************************
// Path3d.cpp
//********************************************
// class CPath3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 15/01/98
// Modified : 15/01/98
// Modified : 07/16/06 - Tom Kerekes
//********************************************

#include "stdafx.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

GLuint CPath3d::shaderProgram[N_GL_CONTEXT] = { 0, 0, 0 };
HGLRC  CPath3d::shaderProgram_gl_context[N_GL_CONTEXT] = { 0, 0, 0 };



//********************************************
// Constructor
//********************************************
CPath3d::CPath3d()
{
	m_ListDone = 0;
	m_Modified = 1;
	m_Name = _T("Path");
	m_Show = 1;
	m_nPointsPerList=10000;
	m_nPointsInList=0;
	m_ToolOffsetValid=false;
	m_RecolorFirst = INT_MAX;	// no recolour pending
	m_RecolorLast = -1;
}

//********************************************
// Destructor
//********************************************
CPath3d::~CPath3d()
{
	Free();
}

//********************************************
// Free
//********************************************
void CPath3d::Free()
{
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	//TRACE("Cleanup mesh %x\n",this);
	m_ArrayVertex.Free();

	// delete all display lists
	int nlists = m_nPointsInList/m_nPointsPerList;

	if ((m_nPointsInList % m_nPointsPerList) != 0)nlists++;

	while (nlists)
	{
		// Erase last list
		nlists--;
		Array2Bufs* p = m_ListArray.GetAt(nlists);
		glBindVertexArray(0); // unbound any vertex array
		glDeleteVertexArrays(1, &p->Array);
		glDeleteBuffers(1, &p->VertexBuf);
		glDeleteBuffers(1, &p->ColorBuf);
		m_ListArray.RemoveAt(nlists);
		delete p;
	}

	m_nPointsInList = 0;
	m_RecolorFirst = INT_MAX;	// nothing left to recolour
	m_RecolorLast = -1;
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
}


//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////

// Function to create shader program
GLuint CPath3d::createShaderProgram() {

	const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in int aColorIndex;

uniform mat4 transform;

flat out int colorIndex;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    colorIndex = aColorIndex;
}
)";

	const char* fragmentShaderSource = R"(
#version 330 core
flat in int colorIndex;
out vec4 FragColor;

uniform vec3 colors[9];	// N_PATH_COLORS (SceneGraph3d.h)

void main()
{
    FragColor = vec4(colors[colorIndex], 1.0);
}
)";
	return BuildShader(vertexShaderSource, fragmentShaderSource);
}


GLfloat CPath3d::ColorToFloat(unsigned char c)
{
	if (c == 0x80) return 0.5f;
	return (GLfloat)c/255.0f;
}

unsigned char CPath3d::GetColorIndexCached(CColor *c)
{
	static unsigned char r=0, g=0, b=0;
	static unsigned char ci = WHITE_COLOR_INDEX; // White index

	if (r == c->r() && g == c->g() && b == c->b())
		return ci;

	GLfloat rf = ColorToFloat(c->r());
	GLfloat gf = ColorToFloat(c->g());
	GLfloat bf = ColorToFloat(c->b());

	for (int i=0; i<sizeof(colorlookup)/sizeof(GLfloat)/3; i++)
	{
		if (colorlookup[i][0] == rf &&
			colorlookup[i][1] == gf &&
			colorlookup[i][2] == bf)
		{
			r = c->r();
			g = c->g();
			b = c->b();
			ci = i;
			return (unsigned char)i;
		}
	}
	return WHITE_COLOR_INDEX; // white index
}

//********************************************
// BuildList
//********************************************
int CPath3d::glBuildList()
{
	unsigned int i;

	//TRACE(" Start building list ...\n");

	// Check for valid Path
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock(); // to be safe lock for the entire function

	if(m_ArrayVertex.GetSize() == 0)
	{
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 0;
	}

	if (!m_Modified && m_ListDone && !RecolorPending())
	{
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 0;
	}

	// Re-upload the colour buffers of already-built lists that contain
	// recoloured vertices (see MarkRecolor) -- including the last, partial list:
	// it is only rebuilt when vertices are added, which no longer happens once
	// the interpreter has read the whole file (or ever, in a Simulate playback).
	if (RecolorPending())
	{
		int nBuilt = (int)m_ListArray.GetSize();
		int k0 = m_RecolorFirst / (int)m_nPointsPerList;
		int k1 = m_RecolorLast / (int)m_nPointsPerList;
		if (k1 > nBuilt - 1) k1 = nBuilt - 1;
		if (k0 < 0) k0 = 0;
		if (k0 <= k1)
		{
			unsigned char *ci = new unsigned char[m_nPointsPerList];
			int nVert = (int)m_ArrayVertex.GetSize();
			for (int k = k0; k <= k1; k++)
			{
				Array2Bufs *p = m_ListArray.GetAt(k);
				int first = k * (int)m_nPointsPerList;
				// never read past the vertices that still exist: the path can shrink
				// under a built list (e.g. RemovePathEnd after a Halt)
				int cnt = p->NbVertex;
				if (cnt > nVert - first) cnt = nVert - first;
				if (cnt <= 0) continue;
				for (int j = 0; j < cnt; j++)
					ci[j] = GetColorIndexCached(m_ArrayVertex[first + j]->GetColor());
				glBindBuffer(GL_ARRAY_BUFFER, p->ColorBuf);
				glBufferSubData(GL_ARRAY_BUFFER, 0, cnt, ci);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			delete[] ci;
		}
		m_RecolorFirst = INT_MAX;
		m_RecolorLast = -1;

		if (!m_Modified && m_ListDone)
		{
			if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
			return 1;
		}
	}

	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();

	if (!NbVertex)
	{
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 0;
	}

	// if latest list is not completely full delete it
	int nlists = m_nPointsInList/m_nPointsPerList;


//	CString Ver = glGetString(GL_VERSION);  


	if ((m_nPointsInList % m_nPointsPerList) != 0)
	{
		// Erase last list
		Array2Bufs *p=m_ListArray.GetAt(nlists);
		glBindVertexArray(0); // unbound any vertex array
		glDeleteVertexArrays(1, &p->Array);
		glDeleteBuffers(1, &p->VertexBuf);
		glDeleteBuffers(1, &p->ColorBuf);
		m_ListArray.RemoveAt(nlists);
		delete p;
		m_nPointsInList = nlists * m_nPointsPerList;
	}

	// Vertex data for the line strip in 3D
	GLfloat * vertices = new GLfloat[3 * m_nPointsPerList];
	unsigned char* colori = new unsigned char[m_nPointsPerList];

	int VertexInThisList = 0;
	// loop untill all the points are in the lists
	while (m_nPointsInList<NbVertex)  
	{
		// path
		
		GLfloat *vp = vertices;
		unsigned char *cp = colori;
		CVertex3dFast *pNew;

		// put into vertices and colors arrays
		int n = 0;
		for (i = m_nPointsInList; (i < NbVertex && i < m_nPointsInList + m_nPointsPerList); i++)
		{
			pNew = m_ArrayVertex[i];

			*vp++ = pNew->x();
			*vp++ = pNew->y();
			*vp++ = pNew->z();

			*cp++ = GetColorIndexCached(pNew->GetColor());
			n++;
		}

		Array2Bufs* p = new Array2Bufs;

		glGenVertexArrays(1, &p->Array);

		glGenBuffers(1, &p->VertexBuf);
		glGenBuffers(1, &p->ColorBuf);

		if (p->Array == 0 || p->VertexBuf == 0 || p->ColorBuf == 0)
		{
			MessageBox(NULL, _T("CPath3d::BuildList : unable to build DrawList"), _T("Error"), MB_OK);
			if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
			return 0;
		}

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(p->Array);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, p->VertexBuf);
		glBufferData(GL_ARRAY_BUFFER, n*4*3, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// Color index buffer (Must use IPointer version for integer data)
		glBindBuffer(GL_ARRAY_BUFFER, p->ColorBuf);
		glBufferData(GL_ARRAY_BUFFER, n*1, colori, GL_STATIC_DRAW);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		p->NbVertex = n;

		m_ListArray.Add(p);

		m_nPointsInList=i;

		glBindVertexArray(0); // unbound any vertex array
	}


	delete vertices;
	delete colori;

	// Lists are done now
	m_ListDone = 1;
	
	// only set as un modified if nothing changed in the mean time
	if(m_ArrayVertex.GetSize() == NbVertex)
	{
		m_Modified = 0;
	}
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();

	return 1;
}

//********************************************
// Draw
//********************************************
int CPath3d::glDraw()
{
	if(!m_Show)
		return 0;

	if(m_ArrayVertex.GetSize() == 0)
		return 0;

	// Build list at first (or re-upload recoloured lists)
	if(!m_ListDone || m_Modified || RecolorPending())
		glBuildList();


	HGLRC Context = wglGetCurrentContext();  // check if we already have a shader in this context
	if (shaderProgram_gl_context[0] != Context &&
		shaderProgram_gl_context[1] != Context &&
		shaderProgram_gl_context[2] != Context)  // no matching context?
	{
		// push second to third
		shaderProgram_gl_context[2] = shaderProgram_gl_context[1];
		shaderProgram[2] = shaderProgram[1];

		// push first context to second
		shaderProgram_gl_context[1] = shaderProgram_gl_context[0];
		shaderProgram[1] = shaderProgram[0];

		// Create shader program and save its context
		shaderProgram[0] = createShaderProgram();
		shaderProgram_gl_context[0] = Context;
	}

	GLuint shader = 0;
	if (Context == shaderProgram_gl_context[0])
	{
		shader = shaderProgram[0];
	}
	else if (Context == shaderProgram_gl_context[1])
	{
		shader = shaderProgram[1];
	}
	else if (Context == shaderProgram_gl_context[2])
	{
		shader = shaderProgram[2];
	}

	glUseProgram(shader);


	GLint colorsLocation = glGetUniformLocation(shader, "colors");

	// all palette entries (stock uploaded only 5 of 6, leaving WHITE undefined)
	glUniform3fv(colorsLocation, (GLsizei)(sizeof(colorlookup) / sizeof(colorlookup[0])), &colorlookup[0][0]);

	// Create and set the transformation matrix
	float ModelMat[16], ProjMat[16], ProductMat[16];
	// Get the current model-view matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelMat);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjMat);
	multiplyMatrices(ProductMat, ModelMat, ProjMat);
	GLint transformLocation = glGetUniformLocation(shader, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, ProductMat);


	int nlists = m_ListArray.GetSize();
	for (int i=0; i<nlists; i++)
	{
		Array2Bufs* p = m_ListArray.GetAt(i);

		glBindVertexArray(p->Array);
		glDrawArrays(GL_LINE_STRIP, 0, p->NbVertex);
	}

	glBindVertexArray(0);  // don't use any vertex array

	glUseProgram(0);  // don't use any shader program

	return 1;
}


//////////////////////////////////////////////
// RANGE
//////////////////////////////////////////////

//********************************************
// Range
//********************************************
void CPath3d::Range(int coord, 
										float *min,
										float *max)
{
	ASSERT(coord >= 0 && coord <= 2);
	int NbVertex = m_ArrayVertex.GetSize();

	if (NbVertex==0) return;

	float Min = m_ArrayVertex[0]->Get(coord);
	float Max = Min;
	for(int i=1;i<NbVertex;i++)
	{
		float value = m_ArrayVertex[i]->Get(coord);
		if(value < Min)
			Min = value;
		if(value > Max)
			Max = value;
	}
	*min = Min;
	*max = Max;
}

//********************************************
// Range (apply)
//********************************************
void CPath3d::Range(int coord, 
										float min,
										float max)
{
	float Min,Max;
	Range(coord,&Min,&Max);
	Offset(coord,-Min);
	Scale(coord,(max-min)/(Max-Min));
	Offset(coord,min);
}

//********************************************
// Scale
//********************************************
void CPath3d::Scale(int coord,
										float scale)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Set(coord,m_ArrayVertex[i]->Get(coord) * scale);
	m_Modified = 1;
}

//********************************************
// Offset
//********************************************
void CPath3d::Offset(int coord,
										 float offset)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex[i]->Set(coord,m_ArrayVertex[i]->Get(coord) + offset);
	m_Modified = 1;
}



//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Copy
//********************************************
void CPath3d::Copy(CPath3d *pPath)
{
	// Vertices
	int NbVertex = pPath->NbVertex();
	m_ArrayVertex.SetSize(NbVertex);
	for(int i=0;i<NbVertex;i++)
		m_ArrayVertex.SetAt(i,new CVertex3dFast(pPath->GetVertex(i)));

	// Transform
	m_Transform.Copy(pPath->GetTransform());
}


//********************************************
// GetType
//********************************************
int CPath3d::GetType()
{
	return TYPE_PATH3D;
}



//********************************************
// DeleteVertex
//********************************************
int CPath3d::DeleteVertex(CVertex3dFast *pVertex)
{
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	int size = m_ArrayVertex.GetSize();
	for(int i=0;i<size;i++)
	{
		CVertex3dFast *pV = m_ArrayVertex[i];
		if(pV == pVertex)
		{
			m_ArrayVertex.RemoveAt(i);
			delete pVertex;
			if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
			return 1;
		}
	}
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
	return 0;
}

//********************************************
// DeleteVertex
//********************************************
int CPath3d::DeleteVertex(int index)
{
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	if(index < m_ArrayVertex.GetSize())
	{
		CVertex3dFast *pVertex = (CVertex3dFast *)m_ArrayVertex[index];
		m_ArrayVertex.RemoveAt(index);
		delete pVertex;
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 1;
	}
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
	return 0;
}


//
// area=(1/2) x Base x Height. Where the height is an altitude drawn from the base to the opposite angle. 
// This formula makes for a relatively easy calculation of the area of a triangle but it is rather difficult 
// to naturally find a triangle that is given in terms of at least one side (the base) and a height. 
// We typically can determine or are given the sides of a triangle when a triangle is present. 
// formula does exist that can calculate the area of a triangle when all three sides are known.
// This formula is attributed to Heron of Alexandria but can be traced back to Archimedes.
//
// This formula is represented by
// Area=SQRT(s(s-a)(s-b)(s-c)),
// where s=(a+b+c)/2 or perimeter/2. 
//
// Finds distance from a point to a line segment which is either:
// #1 perpendiculat distance if point is "within" the endpoints
// #2 distance to closest endpoint if outside
//
// s0 - s2 is the line
// s1 is the point

double CPath3d::FindDistPointToSegment(CVertex3dFast *s0, CVertex3dFast *s1, CVertex3dFast *s2)
{
	double dx = s1->x() - s0->x();
	double dy = s1->y() - s0->y();
	double dz = s1->z() - s0->z();

	double a = sqrt(dx*dx+dy*dy+dz*dz);

	dx = s2->x() - s1->x();
	dy = s2->y() - s1->y();
	dz = s2->z() - s1->z();

	double b = sqrt(dx*dx+dy*dy+dz*dz);

	dx = s2->x() - s0->x();
	dy = s2->y() - s0->y();
	dz = s2->z() - s0->z();

	double c = sqrt(dx*dx+dy*dy+dz*dz);

	double s = s=(a+b+c)/2;

	// if dist from beg to end is tiny, then 
	//    if both other sides tiny treat as collinear

	if (c <= 0.0)		
	{
		if (a<b) return a;
		else     return b;
	}

	double h,v = s*(s-a)*(s-b)*(s-c);

	if (v > 0.0)
		h = 2.0 * sqrt(v)/c;
	else
		h=0.0;

	if (a*a - h*h > c*c || b*b - h*h > c*c)
	{
		// point is outside the edge
		// return distance to closest endpoint

		if (a<b) return a;
		else     return b;
	}
	return h;
}





//********************************************
// DeleteVertex at the end that are flagged with
// sequence_number >= sequence_number
//********************************************
int CPath3d::RemovePathEnd(int sequence_number, int ID, double x, double y, double z)
{
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	int n = m_ArrayVertex.GetSize()-1;

	CGCodeInterpreter *GC = TheFrame->GCodeDlg.Interpreter;


	setup_pointer p=GC->p_setup;

	float xtool = GC->UserUnitsToInchesX(p->tool_table[p->selected_tool_slot].xoffset);
	float ytool = GC->UserUnitsToInchesX(p->tool_table[p->selected_tool_slot].yoffset);
	float ztool = GC->UserUnitsToInchesX(p->tool_table[p->selected_tool_slot].length);

	x -= xtool;
	y -= ytool;
	z -= ztool;


	// first verify that the line number is there
	while (n>=0)
	{
		int SeqNum = GetVertex(n)->Get_sequence_number();
		if (SeqNum == sequence_number) break;
		n--;
	}

	if (n<0)
	{
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 1;
	}


	// now delete until we find the sequence_number
	n = m_ArrayVertex.GetSize()-1;
	while (n>=0)
	{
		if (GetVertex(n)->Get_sequence_number() == sequence_number) break;
		DeleteVertex(n);
		n--;
	}

	if (n<0)
	{
		if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
		return 1;
	}

	// find which segment we nearly pass through by
	// finding the minimum distance to all of them

	double mindist=1e99;
	int mini=0;
	n = m_ArrayVertex.GetSize()-2;
	CVertex3dFast Point(x,y,z);

	while (n>=0)
	{
		double dist = FindDistPointToSegment(GetVertex(n),&Point,GetVertex(n+1));
		
		if (dist < mindist)
		{
			mindist = dist;
			mini = n;
		}
			
		if (GetVertex(n)->Get_sequence_number() != sequence_number ||
			GetVertex(n)->GetID() == ID) break;
		
		n--;
	}



	// now delete all of the line number
	// or until we find the ID

	n = m_ArrayVertex.GetSize()-1;
	while (n>=0)
	{
		if (GetVertex(n)->Get_sequence_number() != sequence_number ||
			GetVertex(n)->GetID() == ID) break;

		if (n == mini+1)
		{
			// instead of deleting the last segment
			// that goes through the point
			// modify it to go to the new point
			GetVertex(n)->Set(x,y,z);
			break;
		}

		DeleteVertex(n);
		n--;
	}

	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();

	if (n<0) return 1;
	return 0;
}

void CPath3d::AddVertexTool(CVertex3dFast *pVertex)
{ 
	CGCodeInterpreter *GC = TheFrame->GCodeDlg.Interpreter;
	setup_pointer p=GC->p_setup;

	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	float xtool = GC->UserUnitsToInchesX(p->tool_table[p->selected_tool_slot].xoffset);
	float ytool = GC->UserUnitsToInches(p->tool_table[p->selected_tool_slot].yoffset);
	float ztool = GC->UserUnitsToInches(p->tool_table[p->selected_tool_slot].length);

	float x = pVertex->x();
	float y = pVertex->y();
	float z = pVertex->z();

	// check if tool offset changed
	if (m_ToolOffsetValid && 
		(m_LastToolOffsetX != xtool || m_LastToolOffsetY != ytool || m_LastToolOffsetZ != ztool))
	{
		// add in a discontinuity jump to the new Tool offset position
		CVertex3dFast *JumpVertex = new CVertex3dFast(m_LastToolPositionX - xtool,
													  m_LastToolPositionY - ytool,
													  m_LastToolPositionZ - ztool,
													  TheFrame->GCodeDlg.m_ColorJump,
													  pVertex->Get_sequence_number(),0);
		m_ArrayVertex.Add(JumpVertex); 
	}

	pVertex->x(x - xtool);
	pVertex->y(y - ytool);
	pVertex->z(z - ztool);

	m_ArrayVertex.Add(pVertex); 

	m_LastToolOffsetX=xtool;  // save everything in case the tool position changes
	m_LastToolOffsetY=ytool;
	m_LastToolOffsetZ=ztool;
	m_LastToolPositionX=x;
	m_LastToolPositionY=y;
	m_LastToolPositionZ=z;
	m_ToolOffsetValid=true;
	m_Modified=1; 
	if (TheFrame) TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
}



