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

#ifndef WED_MARQUEETOOL_H
#define WED_MARQUEETOOL_H

#include "WED_HandleToolBase.h"
#include "IControlHandles.h"
#include "IOperation.h"

class	WED_MarqueeTool : public WED_HandleToolBase, public virtual IControlHandles {
public:

						 WED_MarqueeTool(
										const char *			tool_name,
										GUI_Pane *				host,
										WED_MapZoomerNew *		zoomer,
										IResolver *				resolver);
	virtual				~WED_MarqueeTool();


	// CONTROL HANDLE INTERFACE:
	virtual		void	BeginEdit(void);
	virtual		void	EndEdit(void);

	virtual		intptr_t		CountEntities(void) const;
	virtual		intptr_t	GetNthEntityID(intptr_t n) const;

	virtual		intptr_t		CountControlHandles(intptr_t id						  ) const;
	virtual		void	GetNthControlHandle(intptr_t id, intptr_t n, intptr_t * active, HandleType_t * con_type, Point2 * p, Vector2 * direction, float * radius) const;

	virtual		intptr_t		GetLinks		    (intptr_t id) const;
	virtual		void	GetNthLinkInfo		(intptr_t id, intptr_t n, intptr_t * active, LinkType_t * ltype) const;
	virtual		intptr_t		GetNthLinkSource   (intptr_t id, intptr_t n) const;
	virtual		intptr_t		GetNthLinkSourceCtl(intptr_t id, intptr_t n) const;	// -1 if no bezier ctl point!
	virtual		intptr_t		GetNthLinkTarget   (intptr_t id, intptr_t n) const;
	virtual		intptr_t		GetNthLinkTargetCtl(intptr_t id, intptr_t n) const;

	virtual		bool	PointOnStructure(intptr_t id, const Point2& p) const;

	virtual		void	ControlsMoveBy(intptr_t id, const Vector2& delta, Point2& io_pt);
	virtual		void	ControlsHandlesBy(intptr_t id, intptr_t c, const Vector2& delta, Point2& io_pt);
	virtual		void	ControlsLinksBy	 (intptr_t id, intptr_t c, const Vector2& delta);



//	virtual	int			FindProperty(const char * in_prop) { return -1; }
//	virtual int			CountProperties(void) { return 0; }
//	virtual void		GetNthPropertyInfo(int n, PropertyInfo_t& info) {}
//	virtual	void		GetNthPropertyDict(int n, PropertyDict_t& dict) { }
//	virtual	void		GetNthPropertyDictItem(int n, int e, string& item) { }

//	virtual void		GetNthProperty(int n, PropertyVal_t& val) { }
//	virtual void		SetNthProperty(int n, const PropertyVal_t& val) { }

	virtual	const char *		GetStatusText(void) { return NULL; }

//	virtual void *		QueryInterface(const char * class_id);

private:

	virtual	EntityHandling_t	TraverseEntity(IGISEntity * ent, int pt_sel) { return ent_AtomicOrContainer; }

//				void	GetEntityInternal(vector<IGISEntity *>& e);
				bool	GetTotalBounds(void) const;
				void	ApplyRescale(const Bbox2& old_bounds, const Bbox2& new_bounds);
				void	ApplyRotate(const Point2& ctr, double angle);

			int			mInEdit;
			bool		mIsRotate;
			Point2		mRotateCtr;
			Point2		mRotatePt;

	mutable	Bbox2		mCacheBounds;
	mutable long long	mCacheKeyArchive;
	mutable bool		mCacheIconic;

};

#endif /* WED_MARQUEETOOL_H */
