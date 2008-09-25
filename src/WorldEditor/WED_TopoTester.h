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

#ifndef WED_TOPOTESTER_H
#define WED_TOPOTESTER_H

#include "WED_MapTool.h"
#include "MapDefs.h"

class	WED_TopoTester : public WED_MapTool {
public:

					WED_TopoTester(WED_MapZoomer * inZoomer);
	virtual			~WED_TopoTester();

	virtual	void	DrawFeedbackUnderlay(
							bool				inCurrent);
	virtual	void	DrawFeedbackOverlay(
							bool				inCurrent);
	virtual	bool	HandleClick(
							XPLMMouseStatus		inStatus,
							int 				inX,
							int 				inY,
							int 				inButton);

	virtual int		GetNumProperties(void);
	virtual	void	GetNthPropertyName(int, string&);
	virtual	double	GetNthPropertyValue(int);
	virtual	void	SetNthPropertyValue(int, double);

	virtual	int		GetNumButtons(void);
	virtual	void	GetNthButtonName(int, string&);
	virtual	void	NthButtonPressed(int);

	virtual	char *	GetStatusText(void);

private:

		Point2					mAnchor;
		Point2					mTarget;
		GISHalfedge *			mAnchorHint;
		Pmwx::Locate_type		mAnchorLoc;

		Point2					mFound;
		Pmwx::Locate_type		mFoundLoc;
		GISHalfedge *			mFoundHint;
		bool					mRayShoot;

};

#endif