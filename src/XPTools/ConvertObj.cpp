/*
 * Copyright (c) 2007, Laminar Research.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "version.h"
#include "XObjReadWrite.h"
#include "XObjDefs.h"
#include "ObjUtils.h"
#include "XObjWriteEmbedded.h"
#include "ObjConvert.h"
//#include "XUtils.h"

#include "ConvertObjDXF.h"
#include "ConvertObj3DS.h"


#define kFeetToMeters			0.3048
#define	kInchesToMeters			(0.3048 / 12.0)
#define kMetersToFeet (1.0 / kFeetToMeters)
#define kMetersToInches (1.0 / kInchesToMeters)

enum {
	unit_Inches,
	unit_Feet,
	unit_Meters,

	poly_CW,
	poly_CCW,

	axis_Y,
	axis_Z,
	save_OBJ7,
	save_OBJ8
	
};

static	int	gUnits = unit_Meters;
static	int	gCenterH = 0;
static	int	gFlipX = 0;
static	int	gFlipY = 0;
static	int	gFlipZ = 0;
static	int	gPoly = poly_CCW;
static	int	gAxis = axis_Z;
static	int	gSave = save_OBJ8;

static int	gOptimize = 0;

void	PostProcessVertex(float v[3], bool inReverse)
{
	float	nv[3];

	if (gAxis == axis_Y)
	{
		nv[0] = v[0];
		nv[1] = v[1];
		nv[2] = v[2];
	} else {
		nv[0] = -v[0];
		nv[1] =  v[2];
		nv[2] =  v[1];
	}

	if (gFlipX)	nv[0] = -nv[0];
	if (gFlipY)	nv[1] = -nv[1];
	if (gFlipZ)	nv[2] = -nv[2];

	if (gUnits == unit_Feet)
	{
		nv[0] *= (inReverse ? kMetersToFeet : kFeetToMeters);
		nv[1] *= (inReverse ? kMetersToFeet : kFeetToMeters);
		nv[2] *= (inReverse ? kMetersToFeet : kFeetToMeters);
	}
	if (gUnits == unit_Inches)
	{
		nv[0] *= (inReverse ? kMetersToInches : kInchesToMeters);
		nv[1] *= (inReverse ? kMetersToInches : kInchesToMeters);
		nv[2] *= (inReverse ? kMetersToInches : kInchesToMeters);
	}

	v[0] = nv[0];
	v[1] = nv[1];
	v[2] = nv[2];
}

void	PostProcessObj(XObj& ioObj, bool inReverse)
{
	for (vector<XObjCmd>::iterator cmd = ioObj.cmds.begin(); cmd != ioObj.cmds.end(); ++cmd)
	{
		for (vector<vec_tex>::iterator st = cmd->st.begin(); st != cmd->st.end(); ++st)
		{
			PostProcessVertex(st->v, inReverse);
		}
		for (vector<vec_rgb>::iterator rgb = cmd->rgb.begin(); rgb != cmd->rgb.end(); ++rgb)
		{
			PostProcessVertex(rgb->v, inReverse);
		}
	}

	if (gCenterH)
	{
		float sphere[4];
		GetObjBoundingSphere(ioObj, sphere);
		OffsetObject(ioObj, -sphere[0], 0.0, -sphere[2]);	// DO NOT center on Y!!
	}
}

void	XGrindFile(const char * inConvertFlag, const char * inSrcFile, const char * inDstFile)
{
	XObj	obj;
	XObj8	obj8;
	bool success = false;

	if(strcmp(inConvertFlag,"--obj23ds")==0)
	{
			 if (XObj8Read(inSrcFile, obj8))					Obj8ToObj7(obj8, obj);
		else if (!XObjRead(inSrcFile, obj))					{ printf("Error: unable to open OBJ file %s\n",inSrcFile); exit(1); }

		PostProcessObj(obj, true);
		if (!WriteObj3DS(inDstFile, obj,gPoly == poly_CCW))	{ printf("Error: unable to write 3DS file %s\n", inDstFile); exit(1); }
	}

	if(strcmp(inConvertFlag,"--obj2dxf")==0)
	{
			 if (XObj8Read(inSrcFile, obj8))					Obj8ToObj7(obj8, obj);
		else if (!XObjRead(inSrcFile, obj))						{ printf("Error: unable to open OBJ file %s\n",inSrcFile); exit(1); }

		PostProcessObj(obj, true);
		if (!WriteObjDXF(inDstFile, obj,gPoly == poly_CCW))		{ printf("Error: unable to write DXF file %s\n", inDstFile); exit(1); }
	}

	if(strcmp(inConvertFlag,"--obj2obj")==0)
	{
		if (gSave == save_OBJ8)
		{
				 if (XObjRead(inSrcFile, obj))					Obj7ToObj8(obj,obj8);
			else if (!XObj8Read(inSrcFile, obj8))				{ printf("Error: unable to open OBJ file %s\n",inSrcFile); exit(1); }

			if(gOptimize)
				Obj8_Optimize(obj8);
			if (!XObj8Write(inDstFile, obj8))					{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}
		else
		{
				 if (XObj8Read(inSrcFile, obj8))				Obj8ToObj7(obj8,obj);
			else if (!XObjRead(inSrcFile, obj))				{ printf("Error: unable to open OBJ file %s\n",inSrcFile); exit(1); }

			if (!XObjWrite(inDstFile, obj))					{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}
	}
#if PHONE
	if(strcmp(inConvertFlag,"--obj2obe")==0)
	{
			 if (XObjRead(inSrcFile, obj))					Obj7ToObj8(obj,obj8);
		else if (!XObj8Read(inSrcFile, obj8))				{ printf("Error: unable to open OBJ file %s\n",inSrcFile); exit(1); }

		if(gOptimize)
			Obj8_Optimize(obj8);
		if (!XObjWriteEmbedded(inDstFile, obj8))			{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
	}
#endif

	if(strcmp(inConvertFlag,"--3ds2obj")==0)
	{
		if (!ReadObj3DS(inSrcFile, obj, gPoly == poly_CCW))	{ printf("Error: unable to read DXF file %s\n", inSrcFile); exit(1); }
		PostProcessObj(obj, false);

		if (gSave == save_OBJ8)
		{
			Obj7ToObj8(obj,obj8);

			if(gOptimize)
				Obj8_Optimize(obj8);

			if (!XObj8Write(inDstFile, obj8))				{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}
		else
		{
			if (!XObjWrite(inDstFile, obj))					{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}

	}
	if(strcmp(inConvertFlag,"--dxf2obj")==0)
	{
		if (!ReadObjDXF(inSrcFile, obj, gPoly == poly_CCW))	{ printf("Error: unable to read DXF file %s\n", inSrcFile); exit(1); }
		PostProcessObj(obj, false);

		if (gSave == save_OBJ8)
		{
			Obj7ToObj8(obj,obj8);

			if(gOptimize)
				Obj8_Optimize(obj8);

			if (!XObj8Write(inDstFile, obj8))				{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}
		else
		{
			if (!XObjWrite(inDstFile, obj))					{ printf("Error: unable to write OBJ file %s\n",inDstFile); exit(1); }
		}
	}
}

//void	XGrindFile(const char * inConvertFlag, const char * inSrcFile, const char * inDstFile);

int main(int argc, char * argv[])
{
	if(argc == 2 && !strcmp(argv[1],"--version"))
	{
		print_product_version("ObjConverter", OBJCONVERT_VER, OBJCONVERT_EXTRAVER);
		return 0;
	}
	if(argc == 2 && !strcmp(argv[1],"--auto_config"))
	{
		printf("CMD .obj .3ds \"%s\" CO_UNITS CO_CENTER CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --obj23ds \"INFILE\" \"OUTFILE\"\n", argv[0]);
		printf("CMD .obj .dxf \"%s\" CO_UNITS CO_CENTER CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --obj2dxf \"INFILE\" \"OUTFILE\"\n", argv[0]);
		printf("CMD .obj _new.obj \"%s\" CO_UNITS CO_CENTER CO_OPTIMIZE CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --obj2obj \"INFILE\" \"OUTFILE\"\n", argv[0]);
#if PHONE
		printf("CMD .obj .obe \"%s\" CO_UNITS CO_CENTER CO_OPTIMIZE CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --obj2obe \"INFILE\" \"OUTFILE\"\n", argv[0]);
#endif
		printf("CMD .dxf .obj \"%s\" CO_UNITS CO_CENTER CO_OPTIMIZE CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --dxf2obj \"INFILE\" \"OUTFILE\"\n", argv[0]);
		printf("CMD .3ds .obj \"%s\" CO_UNITS CO_CENTER CO_OPTIMIZE CO_FLIPX CO_FLIPY CO_FLIPZ CO_CCW CO_AXIS CO_OBJ8 --3ds2obj \"INFILE\" \"OUTFILE\"\n", argv[0]);
		printf("OPTIONS ObjConverter\n");
		printf("RADIO CO_UNITS 0 --inches Inches\n");
		printf("RADIO CO_UNITS 0 --feet Feet\n");
		printf("RADIO CO_UNITS 1 --meters Meters\n");
		printf("DIV\n");
		printf("CHECK CO_CENTER 0 --center Center Object Horizontally\n");
		printf("CHECK CO_OPTIMIZE 1 --optimize Optimize Vertices and Indices\n");
		printf("DIV\n");
		printf("CHECK CO_FLIPX 0 --flip_x Flip X\n");
		printf("CHECK CO_FLIPY 0 --flip_y Flip Y\n");
		printf("CHECK CO_FLIPZ 0 --flip_z Flip Z\n");
		printf("DIV\n");
		printf("RADIO CO_CCW 0 --polygon_cw Polygons are CW\n");
		printf("RADIO CO_CCW 1 --polygon_ccw Polygons are CCW\n");
		printf("DIV\n");
		printf("RADIO CO_AXIS 1 --axis_y Y Axis is up\n");
		printf("RADIO CO_AXIS 0 --axis_z Z Axis is up\n");
		printf("DIV\n");
		printf("RADIO CO_OBJ8 0 --obj7 Write OBJ version 7\n");
		printf("RADIO CO_OBJ8 1 --obj8 Write OBJ version 8\n");
		return 0;
	}

	if (argc < 4) { printf("Usage: %s [options ...] --conversion input_file output_file\n       %s --version\n",argv[0],argv[0]); exit(1); }
	for (int a = 1; a < argc-3; ++a)
	{
			 if (!strcmp(argv[a],"--inches"))		gUnits = unit_Inches;
		else if (!strcmp(argv[a],"--feet"))			gUnits = unit_Feet;
		else if (!strcmp(argv[a],"--meters"))		gUnits = unit_Meters;

		else if (!strcmp(argv[a],"--center"))		gCenterH = 1;
		else if (!strcmp(argv[a],"--optimize"))		gOptimize = 1;
		else if (!strcmp(argv[a],"--flip_x"))		gFlipX = 1;
		else if (!strcmp(argv[a],"--flip_y"))		gFlipY = 1;
		else if (!strcmp(argv[a],"--flip_z"))		gFlipZ = 1;

		else if (!strcmp(argv[a],"--polygon_ccw"))	gPoly = poly_CCW;
		else if (!strcmp(argv[a],"--polygon_cw"))	gPoly = poly_CW;

		else if (!strcmp(argv[a],"--axis_y"))		gAxis = axis_Y;
		else if (!strcmp(argv[a],"--axis_z"))		gAxis = axis_Z;

		else if (!strcmp(argv[a],"--obj7"))			gSave = save_OBJ7;
		else if (!strcmp(argv[a],"--obj8"))			gSave = save_OBJ8;

		else { printf("Unknown option %s\n",argv[a]); exit(1); }
	}
	XGrindFile(argv[argc-3],argv[argc-2],argv[argc-1]);
}

