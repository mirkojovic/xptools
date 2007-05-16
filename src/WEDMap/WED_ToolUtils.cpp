#include "WED_ToolUtils.h"
#include "ISelection.h"
#include "WED_Thing.h"
#include "GUI_Pane.h"
#include "IResolver.h"
#include "WED_Airport.h"
#include <list>

#include "GUI_Clipboard.h"
#include "WED_Group.h"
#include "WED_AirportBeacon.h"
#include "WED_AirportBoundary.h"
#include "WED_AirportChain.h"
#include "WED_AirportNode.h"
#include "WED_AirportSign.h"
#include "WED_Helipad.h"
#include "WED_LightFixture.h"
#include "WED_RampPosition.h"
#include "WED_Runway.h"
#include "WED_RunwayNode.h"
#include "WED_Sealane.h"
#include "WED_Taxiway.h"
#include "WED_TowerViewpoint.h"
#include "WED_Windsock.h"


using std::list;

// FIRST thing is root, last is WHO
static void GetLineage(WED_Thing * who, list<WED_Thing *>& parents)
{
	parents.clear();
	while(who)
	{
		parents.push_front(who);
		who = who->GetParent();
	}
}

static WED_Thing *	FindCommonParent(WED_Thing * a, WED_Thing * b)
{
	if (a == NULL) return b->GetParent();
	if (b == NULL) return a->GetParent();
	list<WED_Thing *> a_lin, b_lin;
	GetLineage(a,a_lin);
	GetLineage(b,b_lin);
	
	WED_Thing * best = NULL;
	
	list<WED_Thing *>::iterator a_it = a_lin.begin(), b_it = b_lin.begin();
	while (a_it != a_lin.end() && b_it != b_lin.end() && *a_it == *b_it)
	{
		best = *a_it;
		++a_it, ++b_it;
	}

	return best;
}



WED_Thing *	WED_FindParent(ISelection * isel, WED_Thing * require_this, WED_Thing * backup_choice)
{
	vector<IUnknown *> sel;
	isel->GetSelectionVector(sel);
	
	WED_Thing * common_parent = NULL;
	
	if (sel.empty()) return backup_choice;
	
	for (vector<IUnknown *>::iterator iter = sel.begin(); iter != sel.end(); ++iter)
	{
		WED_Thing * obj = SAFE_CAST(WED_Thing, *iter);
		if (obj)
		{
			WED_Thing * new_parent = FindCommonParent(obj, common_parent);
			DebugAssert(new_parent);
			common_parent = new_parent;
		}
	}

	DebugAssert(common_parent);

	if (require_this)
	if (FindCommonParent(common_parent, require_this) != require_this)
		return require_this;
		
	return common_parent;	
}


WED_Airport *		WED_GetCurrentAirport(IResolver * resolver)
{
	return SAFE_CAST(WED_Airport,resolver->Resolver_Find("choices.airport"));
}

void			WED_SetCurrentAirport(IResolver * resolver, WED_Airport * airport)
{
	IDirectoryEdit * keys = SAFE_CAST(IDirectoryEdit, resolver->Resolver_Find("choices"));
	keys->Directory_Edit("airport", airport);
}


ISelection *	WED_GetSelect(IResolver * resolver)
{
	return SAFE_CAST(ISelection,resolver->Resolver_Find("selection"));
}

WED_Thing *	WED_GetWorld(IResolver * resolver)
{
	return SAFE_CAST(WED_Thing,resolver->Resolver_Find("world"));
}

static void	WED_GetSelectionInOrderRecursive(ISelection * sel, WED_Thing * who, vector<WED_Thing *>& out_sel)
{
	if (sel->IsSelected(who)) out_sel.push_back(who);
	int c = who->CountChildren();
	for (int n = 0; n < c; ++n)
		WED_GetSelectionInOrderRecursive(sel, who->GetNthChild(n), out_sel);
}

void	WED_GetSelectionInOrder(IResolver * resolver, vector<WED_Thing *>& out_sel)
{
	ISelection * sel = WED_GetSelect(resolver);
	WED_Thing * wrl = WED_GetWorld(resolver);
	out_sel.clear();
	WED_GetSelectionInOrderRecursive(sel, wrl, out_sel);
}

bool			WED_IsSelectionNested(IResolver * resolver)
{
	ISelection * sel = WED_GetSelect(resolver);
	return sel->IterateSelection(Iterate_HasSelectedParent, sel);
}

int	Iterate_RequiresAirport(IUnknown * what, void * ref)
{
	if (dynamic_cast<WED_AirportBeacon *>(what)) return 1;
	if (dynamic_cast<WED_AirportBoundary *>(what)) return 1;
	if (dynamic_cast<WED_AirportChain *>(what)) return 1;
	if (dynamic_cast<WED_AirportNode *>(what)) return 1;
	if (dynamic_cast<WED_AirportSign *>(what)) return 1;
	if (dynamic_cast<WED_Helipad *>(what)) return 1;
	if (dynamic_cast<WED_LightFixture *>(what)) return 1;
	if (dynamic_cast<WED_RampPosition *>(what)) return 1;
	if (dynamic_cast<WED_Runway *>(what)) return 1;
	if (dynamic_cast<WED_RunwayNode *>(what)) return 1;
	if (dynamic_cast<WED_Sealane *>(what)) return 1;
	if (dynamic_cast<WED_Taxiway *>(what)) return 1;
	if (dynamic_cast<WED_TowerViewpoint *>(what)) return 1;
	if (dynamic_cast<WED_Windsock *>(what)) return 1;
	return 0;
}

int	Iterate_ChildRequiresAirport(IUnknown * what, void * ref)
{
	if (Iterate_RequiresAirport(what, ref)) return 1;
	WED_Thing * o = dynamic_cast<WED_Thing *>(what);
	if (o == NULL) return 0;
	if (dynamic_cast<WED_Airport *>(what)) return 0;				// Ben says: if we are an airport, do not eval our kids - their needs are met!
	for (int n = 0; n < o->CountChildren(); ++n)
	if (Iterate_ChildRequiresAirport(o->GetNthChild(n), ref))
		return 1;
	return 0;
}

int	Iterate_IsAirport(IUnknown * what, void * ref)
{
	if (dynamic_cast<WED_Airport *>(what)) return 1;
	return 0;
}

int	Iterate_IsOrParentAirport(IUnknown * what, void * ref)
{
	if (what == NULL) return 0;
	WED_Thing * o = dynamic_cast<WED_Thing *>(what);
	if (o == NULL) return 0;
	while (o)
	{
		if (dynamic_cast<WED_Airport *>(o) != NULL) return 1;
		o = o->GetParent();
	}
	return 0;
}

int	Iterate_IsOrChildAirport(IUnknown * what, void * ref)
{
	if (what == NULL) return 0;
	WED_Thing * o = dynamic_cast<WED_Thing *>(what);
	if (o == NULL) return 0;
	if (dynamic_cast<WED_Airport *>(o) != NULL) return 1;
	
	for (int n = 0; n < o->CountChildren(); ++n)
	{
		if (Iterate_IsOrChildAirport(o->GetNthChild(n), ref))
			return 1;
	}
	return 0;
}


int	Iterate_IsPartOfStructuredObject(IUnknown * what, void * ref)
{
	WED_Thing * who = dynamic_cast<WED_Thing *>(what);
	if (!who) return 0;
	
	WED_Thing * my_parent = who->GetParent();
	if (!my_parent) return 0;
	
	IGISEntity * e = dynamic_cast<IGISEntity *>(my_parent);
	if (!e) return 0;
	
	switch(e->GetGISClass()) {
	case gis_PointSequence:	
	case gis_Line:
	case gis_Line_Width:
	case gis_Ring:
	case gis_Chain:
	case gis_Polygon:	return 1;
	default:			return 0;
	}		
}

int Iterate_IsNotGroup(IUnknown * what, void * ref)
{
	if (dynamic_cast<WED_Group *>(what) == NULL) return 1;
	return 0;
}

int Iterate_ParentMismatch(IUnknown * what, void * ref)
{
	WED_Thing * who = dynamic_cast<WED_Thing *>(what);
	WED_Thing * parent = (WED_Thing *) ref;
	if (who == NULL)				return 1;	
	if (who->GetParent() != parent) return 1;
									return 0;
}

int Iterate_IsParentOf(IUnknown * what, void * ref)					// This object is a parent of (or is) "ref".
{
	WED_Thing * child = dynamic_cast<WED_Thing *>(what);
	WED_Thing * parent = (WED_Thing *) ref;
	
	while(parent)
	{
		if (child == parent) return 1;
		parent = parent->GetParent();
	}
	return 0;	
}

int Iterate_HasSelectedParent(IUnknown * what, void * ref)
{
	WED_Thing * p = dynamic_cast<WED_Thing *>(what);
	if (p == NULL) return 0;
	ISelection * sel = (ISelection *) ref;
	p = p->GetParent();
	while (p)
	{
		if (sel->IsSelected(p)) return 1;
		p = p->GetParent();
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
// DRAG & DROP
//---------------------------------------------------------------------------------------------------------------------------------

static	GUI_ClipType	sSelectionType;

void				WED_RegisterDND(void)
{
	sSelectionType = GUI_RegisterPrivateClipType("WED_Selection");
}

GUI_DragOperation	WED_DoDragSelection(
								GUI_Pane *				pane,
								int						x, 
								int						y,
								int						where[4])
{
	void * dummy = NULL;
	const void *	ptrs[1] = { &dummy };
	int		sizes[1] = { sizeof(dummy) };
	return pane->DoDragAndDrop(x,y,where,
							gui_Drag_Move,
							1,
							&sSelectionType,
							sizes,
							ptrs,
							NULL, NULL);
}

bool				WED_IsDragSelection(
								GUI_DragData *			drag)
{
	if (drag->CountItems() != 1) return false;
	return drag->NthItemHasClipType(0, sSelectionType);
}

