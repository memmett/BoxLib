
#include "hg_projector.H"

#ifdef BL_FORT_USE_UNDERSCORE
#  define   FORT_HGDIV      hgdiv_
#  define   FORT_HGDIV_TERRAIN      hgdiv_terrain_
#  define   FORT_HGFDIV     hgfdiv_
#  define   FORT_HGFDIV_TERRAIN     hgfdiv_terrain_
#  define   FORT_HGEDIV     hgediv_
#  define   FORT_HGEDIV_TERRAIN     hgediv_terrain_
#  define   FORT_HGCDIV     hgcdiv_
#  define   FORT_HGCDIV_TERRAIN     hgcdiv_terrain_
#  define   FORT_HGODIV     hgodiv_
#  define   FORT_HGIDIV     hgidiv_
#  define   FORT_HGDDIV     hgddiv_
#  define   FORT_HGGRAD     hggrad_
#  define   FORT_HGGRAD_TERRAIN     hggrad_terrain_
#  define   FORT_HGAVG      hgavg_
#  define   FORT_HGFAVG     hgfavg_
#  define   FORT_HGEAVG     hgeavg_
#  define   FORT_HGCAVG     hgcavg_
#else
#  define   FORT_HGDIV      HGDIV
#  define   FORT_HGDIV_TERRAIN      HGDIV_TERRAIN
#  define   FORT_HGFDIV     HGFDIV
#  define   FORT_HGFDIV_TERRAIN     HGFDIV_TERRAIN
#  define   FORT_HGEDIV     HGEDIV
#  define   FORT_HGEDIV_TERRAIN     HGEDIV_TERRAIN
#  define   FORT_HGCDIV     HGCDIV
#  define   FORT_HGCDIV_TERRAIN     HGCDIV_TERRAIN
#  define   FORT_HGODIV     HGODIV
#  define   FORT_HGIDIV     HGIDIV
#  define   FORT_HGDDIV     HGDDIV
#  define   FORT_HGGRAD     HGGRAD
#  define   FORT_HGGRAD_TERRAIN     HGGRAD_TERRAIN
#  define   FORT_HGAVG      HGAVG
#  define   FORT_HGFAVG     HGFAVG
#  define   FORT_HGEAVG     HGEAVG
#  define   FORT_HGCAVG     HGCAVG
#endif

extern "C" 
{
    
#if (BL_SPACEDIM == 1)
#error not relevant
#elif (BL_SPACEDIM == 2 || BL_SPACEDIM == 3)
    void FORT_HGGRAD_TERRAIN(RealPS, intS, Real*, intS, intS);
    void FORT_HGDIV_TERRAIN(Real*, intS, RealPS, intS, intS);
    void FORT_HGFDIV_TERRAIN(Real*, intS, RealPS, intS, RealPS, intS, intS, intRS, int&, int&);
#    if (BL_SPACEDIM == 3)
    void FORT_HGEDIV_TERRAIN(Real*, intS, RealPS, intS, RealPS, intS, intS, intRS, const int*, const int*);
#    endif
    void FORT_HGCDIV_TERRAIN(Real*, intS, RealPS, intS, RealPS, intS, intS, intRS, const int*);
    
#if (BL_SPACEDIM == 2)
    void FORT_HGGRAD(RealPS, intS, Real*, intS, intS, CRealPS, const int&);
    void FORT_HGDIV(Real*, intS, RealPS, intS, intS, CRealPS, const int&, const int&);
    void FORT_HGFDIV(Real*, intS, RealPS, intS, RealPS, intS, intS, CRealPS, intRS, int&, int&, const int&, const int&);
    void FORT_HGODIV(Real*, intS, Real*, Real*, intS, Real*, Real*, intS, intS, Real&, Real&, intRS, int&, int&, const int&);
    void FORT_HGIDIV(Real*, intS, Real*, Real*, intS, Real*, Real*, intS, intS, Real&, Real&, intRS, int&, int&, const int&);
    void FORT_HGDDIV(Real*, intS, Real*, Real*, intS, Real*, Real*, intS, intS, Real&, Real&, intRS, int&, const int&);
    void FORT_HGAVG(Real*, intS, const Real*, intS, intS, const Real&, const int&, const int&);
    void FORT_HGFAVG(Real*, intS, const Real*, intS, const Real*, intS, intS, intRS, int&, int&,
	const Real&, const int&, const int&);
    void FORT_HGCAVG(Real*, intS, const Real*, intS, const Real*, intS, intS, intRS, const int*, const Real&, const int&, const int&);
#  elif (BL_SPACEDIM == 3)
    void FORT_HGGRAD(RealPS, intS, Real*, intS, intS, CRealPS);
    void FORT_HGDIV(Real*, intS, RealPS, intS, intS, CRealPS);
    void FORT_HGFDIV(Real*, intS, RealPS, intS, RealPS, intS, intS, CRealPS, intRS, int&, int&);
    void FORT_HGEDIV(Real*, intS, RealPS, intS, RealPS, intS, intS, CRealPS, intRS, const int*, const int*);
    void FORT_HGCDIV(Real*, intS, RealPS, intS, RealPS, intS, intS, CRealPS, intRS, const int*);
    void FORT_HGAVG(Real*, intS, const Real*, intS, intS);
    void FORT_HGFAVG(Real*, intS, const Real*, intS, const Real*, intS, intS, intRS, int&, int&);
    void FORT_HGEAVG(Real*, intS, const Real*, intS, const Real*, intS, intS, intRS, const int*, const int*);
    void FORT_HGCAVG(Real*, intS, const Real*, intS, const Real*, intS, intS, intRS, const int*);
#  endif
#endif
}

PArray<MultiFab> null_amr_real;

void holy_grail_amr_projector::project(PArray<MultiFab>* u,
				       PArray<MultiFab>& p,
				       PArray<MultiFab>& Coarse_source,
				       PArray<MultiFab>& Sigma,
				       Real H[], Real tol,
				       int Lev_min, int Lev_max,
				       Real scale)
{
    if (Lev_min < 0)
	Lev_min = lev_min_max;
    if (Lev_max < 0)
	Lev_max = Lev_min;
    
    assert(Sigma.length() > 0);
    
    assert(u[      0      ][Lev_min].nGrow() == 1);
    assert(u[      1      ][Lev_min].nGrow() == 1);
    assert(u[BL_SPACEDIM-1][Lev_min].nGrow() == 1);
    
    alloc(p, null_amr_real, Coarse_source, Sigma, H, Lev_min, Lev_max);
    right_hand_side(u, null_amr_real);
    if (singular && Coarse_source.ready() && make_sparse_node_source_solvable) 
    {
	sparse_node_source_adjustment(coarse_source);
    }
    
    solve(tol, scale, 2, 2);
    form_solution_vector(u, Sigma);
    clear();
}

void holy_grail_amr_projector::sync_project(PArray<MultiFab>* u,
					    PArray<MultiFab>& p,
					    PArray<MultiFab>& Coarse_source,
					    PArray<MultiFab>& Sigma,
					    Real H[], Real tol,
					    int Lev_min, int Lev_max,
					    Real scale)
{
    if (Lev_min < 0)
	Lev_min = lev_min_max;
    if (Lev_max < 0)
	Lev_max = Lev_min;
    
    assert(Sigma.length() > 0);
    
    assert(u[      0      ][Lev_min].nGrow() == 1);
    assert(u[      1      ][Lev_min].nGrow() == 1);
    assert(u[BL_SPACEDIM-1][Lev_min].nGrow() == 1);
    
    alloc(p, null_amr_real, Coarse_source, Sigma, H, Lev_min, Lev_max);
    sync_right_hand_side(u);
    if (singular && Coarse_source.ready() && make_sparse_node_source_solvable) 
    {
	sparse_node_source_adjustment(coarse_source);
    }
    
    solve(tol, scale, 2, 2);
    form_solution_vector(u, Sigma);
    clear();
}

void holy_grail_amr_projector::manual_project(PArray<MultiFab>* u,
					      PArray<MultiFab>& p,
					      PArray<MultiFab>& rhs,
					      PArray<MultiFab>& Coarse_source,
					      PArray<MultiFab>& Sigma,
					      bool use_u,
					      Real H[], Real tol,
					      int Lev_min, int Lev_max,
					      Real scale)
{
    if (Lev_min < 0)
	Lev_min = lev_min_max;
    if (Lev_max < 0)
	Lev_max = Lev_min;
    
    assert(Sigma.length() > 0);
    
    if (use_u) 
    {
	assert(u[      0      ][Lev_min].nGrow() == 1);
	assert(u[      1      ][Lev_min].nGrow() == 1);
	assert(u[BL_SPACEDIM-1][Lev_min].nGrow() == 1);
	
	alloc(p, null_amr_real, Coarse_source, Sigma, H, Lev_min, Lev_max);
	if (rhs.length() > 0) 
	{
	    if (type(rhs[Lev_min]) == IntVect::TheNodeVector()) 
	    {
		right_hand_side(u, null_amr_real);
		for (int lev = Lev_min; lev <= Lev_max; lev++)
		    source[lev].plus(rhs[lev], 0, 1, 0);
		if (singular && make_sparse_node_source_solvable) 
		{
		    // Note:  You don't want to do this if rhs is not sparse!
		    sparse_node_source_adjustment(rhs);
		}
	    }
	    else 
	    {
		assert(rhs[Lev_min].nGrow() == 1);
		right_hand_side(u, rhs);
	    }
	}
	else 
	{
	    right_hand_side(u, null_amr_real);
	}
    }
    else 
    {
	assert(rhs.length() > 0);
	assert(rhs[Lev_min].nGrow() == 1);
	
	if (type(rhs[Lev_min]) == IntVect::TheNodeVector()) 
	{
	    alloc(p, rhs, Coarse_source, Sigma, H, Lev_min, Lev_max);
	    if (singular && make_sparse_node_source_solvable) 
	    {
		// Note:  You don't want to do this if rhs is not sparse!
		sparse_node_source_adjustment(rhs);
	    }
	}
	else 
	{
	    alloc(p, null_amr_real, Coarse_source, Sigma, H, Lev_min, Lev_max);
	    // source is set to 0 at this point
	    right_hand_side(0, rhs);
	}
    }
    if (singular && Coarse_source.ready() && make_sparse_node_source_solvable) 
    {
	sparse_node_source_adjustment(coarse_source);
    }
    
    solve(tol, scale, 2, 2);
    form_solution_vector(u, Sigma);
    clear();
}

void holy_grail_amr_projector::sparse_node_source_adjustment(PArray<MultiFab>& sparse_source)
{
    // This routine takes advantage of the sparse structure of
    // the sync source to avoid costly restriction operations.  It
    // is necessary to use the inner_product routine, which weights
    // boundary nodes, since the coarse-fine lev_interface can touch
    // the boundary.  Otherwise a call to sum would suffice.
    
    // Note that the correction is applied to source, not to
    // sparse_source, since the sparse structure of the latter
    // may need to be preserved.
    
    assert(singular);
    assert(make_sparse_node_source_solvable);
    
    Real adjust = 0.0;
    for (int lev = lev_max; lev >= lev_min; lev--) 
    {
	if (sparse_source.defined(lev)) 
	{
	    int mglev = ml_index[lev];
	    corr[mglev].setVal(1.0);
	    adjust += inner_product(sparse_source[lev], corr[mglev]);
	}
	if (lev > lev_min && adjust != 0.0) 
	{
	    for (int i = 0; i < BL_SPACEDIM; i++) 
	    {
		adjust /= gen_ratio[lev-1][i];
	    }
	}
    }
    if (adjust != 0.0) 
    {
	adjust /= mg_domain[ml_index[lev_min]].numPts();
	
	if (pcode >= 2  && ParallelDescriptor::IOProcessor())
	    cout << "Sparse-source solvability adjustment: " << adjust << endl;
	
	for (int lev = lev_min; lev <= lev_max; lev++) 
	{
	    source[lev].plus(-adjust, 0);
	}
    }
}

// This is a combination routine which combines sources from a divergence
// and from a cell-based right hand side S in the proper sequence.  The
// key feature is that both "grid" routines must be called before starting
// the lev_interface calculation, since they trash some lev_interface points.

void holy_grail_amr_projector::right_hand_side(PArray<MultiFab>* u, PArray<MultiFab>& S)
{
    if (u) 
    {
	grid_divergence(u);
    }
    if (S.length() > 0) 
    {
	grid_average(S);
    }
    
    for (int lev = lev_min; lev <= lev_max; lev++) 
    {
	const int mglev = ml_index[lev];
	
	clear_part_interface(source[lev], lev_interface[mglev]);
	
	if (lev > lev_min) 
	{
	    if (u) 
	    {
		interface_divergence(u, lev);
	    }
	    if (S.length() > 0) 
	    {
		interface_average(S, lev);
	    }
	}
    }
}

// Averages cell-based S onto node-based source conservatively
// across the composite mesh.  S must be passed in with a one
// cell wide border.  At inflow boundaries border values should
// be set to correct inflow condition.  Other border values passed
// in may be meaningless, but should not be NaNs.

// This routine will modify the borders of S.  Also, if the problem
// being solved is singular, S will be adjusted so that it integrates
// to 0 to maximum precision.  (It is assumed that any additional
// contribution to the right hand side will also integrate to 0.)

// This is an incomplete routine---interface_average must also be called.

void holy_grail_amr_projector::grid_average(PArray<MultiFab>& S)
{
    assert(S[lev_min].nGrow() == 1);
    
    if (singular) 
    {
	Real adjust = 0.0;
	for (int lev = lev_max; lev > lev_min; lev--) 
	{
	    restrict_level(S[lev-1], S[lev], gen_ratio[lev-1], default_restrictor(), level_interface(), 0);
	}
	for (MultiFabIterator S_mfi(S[lev_min]); S_mfi.isValid(); ++S_mfi)
	{
	    adjust += S_mfi->sum(S_mfi.validbox(), 0);
	}
	ParallelDescriptor::ReduceRealSum(adjust);
	adjust /= mg_domain[ml_index[lev_min]].numPts();
	
	if (pcode >= 2  && ParallelDescriptor::IOProcessor())
	    cout << "Cell-source solvability adjustment: " << adjust << endl;
	
	for (int lev = lev_min; lev <= lev_max; lev++) 
	{
	    S[lev].plus(-adjust, 0);
	}
    }
    
    for (int lev = lev_min; lev <= lev_max; lev++) 
    {
	const int mglev = ml_index[lev];
	const Real hx = h[mglev][0];
	
	fill_borders(S[lev], lev_interface[mglev], boundary.scalar(), -1, m_hg_terrain);
	
	for (MultiFabIterator s_mfi(source[lev]); s_mfi.isValid(); ++s_mfi)
	{
	    DependentMultiFabIterator S_dmfi(s_mfi, S[lev]);
	    const Box& sbox = s_mfi->box();
	    const Box& fbox = S_dmfi->box();
	    const Box& freg = lev_interface[mglev].part_fine(s_mfi.index());
	    Real* sptr = s_mfi->dataPtr();
	    const Real* csptr = S_dmfi->dataPtr();
#if (BL_SPACEDIM == 2)
	    FORT_HGAVG(sptr, DIMLIST(sbox),
		csptr, DIMLIST(fbox), DIMLIST(freg),
		hx, IsRZ(), mg_domain[mglev].bigEnd(0) + 1);
#else
	    FORT_HGAVG(sptr, DIMLIST(sbox), csptr, DIMLIST(fbox), DIMLIST(freg));
#endif
	}
    }
}

// This is an incomplete routine---interface_divergence must also be called.

void holy_grail_amr_projector::grid_divergence(PArray<MultiFab>* u)
{ 
    for (int lev = lev_min; lev <= lev_max; lev++) 
    {
	const int mglev = ml_index[lev];
	const Real hx = h[mglev][0];
	const Real hy = h[mglev][1];
#if (BL_SPACEDIM == 3)
	const Real hz = h[mglev][2];
#endif
	
	for (int i = 0; i < BL_SPACEDIM; i++) 
	{
	    fill_borders(u[i][lev], lev_interface[mglev], boundary.velocity(i), -1, m_hg_terrain);
	}
	
	for (MultiFabIterator s_mfi(source[lev]); s_mfi.isValid(); ++s_mfi)
	{
	    DependentMultiFabIterator u_dmfi_0(s_mfi, u[0][lev]);
	    DependentMultiFabIterator u_dmfi_1(s_mfi, u[1][lev]);
	    const Box& sbox = s_mfi->box();
	    const Box& fbox = u_dmfi_0->box();
	    const Box& freg = lev_interface[mglev].part_fine(s_mfi.index());
	    Real* sptr = s_mfi->dataPtr();
	    Real* u0ptr = u_dmfi_0->dataPtr();
	    Real* u1ptr = u_dmfi_1->dataPtr();
#if (BL_SPACEDIM == 3)
	    DependentMultiFabIterator u_dmfi_2(s_mfi, u[2][lev]);
	    Real* u2ptr = u_dmfi_2->dataPtr();
#endif
	    if (m_hg_terrain)
	    {
		FORT_HGDIV_TERRAIN(sptr, DIMLIST(sbox), D_DECL(u0ptr, u1ptr, u2ptr), DIMLIST(fbox), DIMLIST(freg));
	    }
	    else
	    {
#if (BL_SPACEDIM == 2)
		FORT_HGDIV(sptr, DIMLIST(sbox),
		    u0ptr, u1ptr, DIMLIST(fbox), DIMLIST(freg), hx, hy,
		    IsRZ(), mg_domain[mglev].bigEnd(0) + 1);
#else
		FORT_HGDIV(sptr, DIMLIST(sbox),
		    u0ptr, u1ptr, u2ptr, DIMLIST(fbox), DIMLIST(freg),
		    &hx, &hy, &hz);
#endif
	    }
	}
    }
}

// Obsolete:
void holy_grail_amr_projector::sync_right_hand_side(PArray<MultiFab>* u)
{ 
    for (int lev = lev_min; lev <= lev_max; lev++) 
    {
	source[lev].setVal(0.0);
    }
    
    const int mglev0 = ml_index[lev_min];
    interface_divergence(u, lev_min+1);
    
    if (singular) 
    {
	const int mglev1 = ml_index[lev_min+1];
	restrict_level(source[lev_min], source[lev_min+1], gen_ratio[lev_min],
	    bilinear_restrictor_coarse_class(0, m_hg_terrain), lev_interface[mglev1], mg_boundary);
	work[mglev0].setVal(1.0);
	Real adjustment = inner_product(source[lev_min], work[mglev0]) /
	    mg_domain[ml_index[lev_min]].volume();
	if (pcode >= 2  && ParallelDescriptor::IOProcessor())
	    cout << "Solvability adjustment is " << adjustment << endl;
	for (int lev = lev_min; lev <= lev_max; lev++) 
	{
	    source[lev].plus(-adjustment, 0);
	}
    }
}

void holy_grail_amr_projector::interface_average(PArray<MultiFab>& S, int lev)
{ 
    const int mglev = ml_index[lev];
    const int mgc = ml_index[lev-1];
    const Real hx = h[mglev][0];
    
    const IntVect& rat = gen_ratio[lev-1];
    // PARALLEL
    for (int iface = 0; iface < lev_interface[mglev].nfaces(); iface++) 
    {
	// find a fine grid touching this face
	int igrid = lev_interface[mglev].fgrid(iface, 0);
	if (igrid < 0)
	    igrid = lev_interface[mglev].fgrid(iface, 1);
	const unsigned int geo = lev_interface[mglev].fgeo(iface);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].fflag(iface) )
	    continue;
	// fine grid on just one side
	int idim = lev_interface[mglev].fdim(iface);
	int idir = (geo & level_interface::LOW) ? -1 : 1;
	const Box& sbox = source[lev][igrid].box();
	const Box& fbox = S[lev][igrid].box();
	Box cbox = lev_interface[mglev].face(iface);
	IntVect t = cbox.type();
	if (idir > 0)
	    cbox.growLo(idim, rat[idim]);
	else
	    cbox.growHi(idim, rat[idim]);
	cbox.convert(IntVect::TheCellVector()).coarsen(rat);
	FArrayBox *Scp;
	int jgrid = find_patch(cbox, S[lev-1], 0);
	if (jgrid < 0) 
	{
	    Scp = new FArrayBox(cbox);
	    fill_patch(*Scp, Scp->box(), S[lev-1], lev_interface[mgc], boundary.scalar(), 0);
	}
	else 
	{
	    Scp = &S[lev-1][jgrid];
	    cbox = Scp->box();
	}
	FArrayBox& Sc = *Scp;
	Box creg = lev_interface[mglev].node_face(iface);
	creg.coarsen(rat).grow(t - IntVect::TheUnitVector());
	Real* sptr = source[lev][igrid].dataPtr();
	const Real* Sfptr = S[lev][igrid].dataPtr();
#if (BL_SPACEDIM == 2)
	FORT_HGFAVG(sptr, DIMLIST(sbox),
	    Sc.dataPtr(), DIMLIST(cbox),
	    Sfptr, DIMLIST(fbox), DIMLIST(creg),
	    rat[0], rat[1], idim, idir,
	    hx, IsRZ(), mg_domain[mgc].bigEnd(0) + 1);
#else
	FORT_HGFAVG(sptr, DIMLIST(sbox),
	    Sc.dataPtr(), DIMLIST(cbox),
	    Sfptr, DIMLIST(fbox), DIMLIST(creg),
	    rat[0], rat[1], rat[2], idim, idir);
#endif
	if (jgrid < 0) 
	{
	    delete Scp;
	}
    }
    
    int ga[level_interface::N_CORNER_GRIDS];
    
#if (BL_SPACEDIM == 3)
    // PARALLEL    
    for (int iedge = 0; iedge < lev_interface[mglev].nedges(); iedge++) 
    {
	// find a fine grid touching this edge
	int igrid;
	for (int i = 0; i < level_interface::N_EDGE_GRIDS; i++) 
	{
	    igrid = lev_interface[mglev].egrid(iedge, i);
	    if (igrid >= 0)
		break;
	}
	const unsigned int geo = lev_interface[mglev].geo(1, iedge);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].flag(1, iedge) )
	    continue;
	// fine grid on just one side
	const Box& sbox = source[lev][igrid].box();
	Real* sptr = source[lev][igrid].dataPtr();
	Box cbox = lev_interface[mglev].edge(iedge);
	IntVect t = cbox.type();
	cbox.coarsen(rat).grow(t).convert(IntVect::TheCellVector());
	Box fbox = cbox;
	fbox.refine(rat);
	FArrayBox *Scp;
	int jgrid = find_patch(cbox, S[lev-1], 0);
	if (jgrid < 0) 
	{
	    Scp = new FArrayBox(cbox);
	    fill_patch(*Scp, Scp->box(), S[lev-1], lev_interface[mgc], boundary.scalar(), 0);
	}
	else 
	{
	    Scp = &S[lev-1][jgrid];
	    cbox = Scp->box();
	}
	FArrayBox& Sc = *Scp;
	FArrayBox Sf(fbox);
	fill_patch(Sf, Sf.box(), S[lev], lev_interface[mglev], boundary.scalar(), 0, 1, iedge);
	Box creg = lev_interface[mglev].node_edge(iedge);
	creg.coarsen(rat).grow(t - IntVect::TheUnitVector());
	lev_interface[mglev].geo_array(ga, 1, iedge);
	FORT_HGEAVG(sptr, DIMLIST(sbox),
	    Sc.dataPtr(), DIMLIST(cbox),
	    Sf.dataPtr(), DIMLIST(fbox),
	    DIMLIST(creg), rat[0], rat[1], rat[2], t.getVect(), ga);
	if (jgrid < 0) 
	{
	    delete Scp;
	}
	// fill in the grids on the other sides, if any
	const Box& freg = lev_interface[mglev].node_edge(iedge);
	for (int i = 1; i < level_interface::N_EDGE_GRIDS; i++) 
	{
	    jgrid = lev_interface[mglev].egrid(iedge, i);
	    if (jgrid >= 0 && jgrid != igrid)
		internal_copy(source[lev], jgrid, igrid, freg);
	}
    }
    
#endif
    // PARALLEL    
    for (int icor = 0; icor < lev_interface[mglev].ncorners(); icor++) 
    {
	// find a fine grid touching this corner
	int igrid;
	for (int i = 0; i < level_interface::N_CORNER_GRIDS; i++) 
	{
	    igrid = lev_interface[mglev].cgrid(icor, i);
	    if (igrid >= 0)
		break;
	}
	const unsigned int geo = lev_interface[mglev].geo(0, icor);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].flag(0, icor) )
	    continue;
	// fine grid on just one side
	const Box& sbox = source[lev][igrid].box();
	Real* sptr = source[lev][igrid].dataPtr();
	Box cbox = lev_interface[mglev].corner(icor);
	cbox.coarsen(rat).grow(1).convert(IntVect::TheCellVector());
	Box fbox = cbox;
	fbox.refine(rat);
	FArrayBox *Scp;
	int jgrid = find_patch(cbox, S[lev-1], 0);
	if (jgrid < 0) 
	{
	    Scp = new FArrayBox(cbox);
	    fill_patch(*Scp, Scp->box(), S[lev-1], lev_interface[mgc], boundary.scalar(), 0);
	}
	else 
	{
	    Scp = &S[lev-1][jgrid];
	    cbox = Scp->box();
	}
	FArrayBox& Sc = *Scp;
	FArrayBox Sf(fbox);
	fill_patch(Sf, Sf.box(), S[lev], lev_interface[mglev], boundary.scalar(), 0, 0, icor);
	Box creg = lev_interface[mglev].corner(icor);
	creg.coarsen(rat);
	lev_interface[mglev].geo_array(ga, 0, icor);
#if (BL_SPACEDIM == 2)
	FORT_HGCAVG(sptr, DIMLIST(sbox),
	    Sc.dataPtr(), DIMLIST(cbox),
	    Sf.dataPtr(), DIMLIST(fbox),
	    DIMLIST(creg), rat[0], rat[1], ga,
	    hx, IsRZ(), mg_domain[mgc].bigEnd(0) + 1);
#else
	FORT_HGCAVG(sptr, DIMLIST(sbox),
	    Sc.dataPtr(), DIMLIST(cbox),
	    Sf.dataPtr(), DIMLIST(fbox),
	    DIMLIST(creg), rat[0], rat[1], rat[2], ga);
#endif
	if (jgrid < 0) 
	{
	    delete Scp;
	}
	// fill in the grids on the other sides, if any
	const Box& freg = lev_interface[mglev].corner(icor);
	for (int i = 1; i < level_interface::N_CORNER_GRIDS; i++) 
	{
	    jgrid = lev_interface[mglev].cgrid(icor, i);
	    if (jgrid >= 0 && jgrid != igrid)
		internal_copy(source[lev], jgrid, igrid, freg);
	}
    }
}

void holy_grail_amr_projector::interface_divergence(PArray<MultiFab>* u, int lev)
{ 
    int mglev = ml_index[lev];
    int mgc = ml_index[lev-1];
    Real hx = h[mglev][0];
    Real hy = h[mglev][1];
#if (BL_SPACEDIM == 3)
    Real hz = h[mglev][2];
#endif
    
    const IntVect& rat = gen_ratio[lev-1];
    // PARALLEL
    for (int iface = 0; iface < lev_interface[mglev].nfaces(); iface++) 
    {
	// find a fine grid touching this face
	int igrid = lev_interface[mglev].fgrid(iface, 0);
	if (igrid < 0)
	    igrid = lev_interface[mglev].fgrid(iface, 1);
	const unsigned int geo = lev_interface[mglev].fgeo(iface);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].fflag(iface) )
	    continue;
	// fine grid on just one side
	int idim = lev_interface[mglev].fdim(iface);
	int idir = (geo & level_interface::LOW) ? -1 : 1;
	const Box& sbox = source[lev][igrid].box();
	const Box& fbox = u[0][lev][igrid].box();
	Box cbox = lev_interface[mglev].face(iface);
	IntVect t = cbox.type();
	if (idir > 0)
	    cbox.growLo(idim, rat[idim]);
	else
	    cbox.growHi(idim, rat[idim]);
	cbox.convert(IntVect::TheCellVector()).coarsen(rat);
	FArrayBox *ucp, *vcp;
	const int jgrid = find_patch(cbox, u[0][lev-1], 0);
	if (jgrid < 0) 
	{
	    ucp = new FArrayBox(cbox);
	    vcp = new FArrayBox(cbox);
	    fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
	    fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	}
	else 
	{
	    ucp = &u[0][lev-1][jgrid];
	    vcp = &u[1][lev-1][jgrid];
	    cbox = ucp->box();
	}
	FArrayBox& uc = *ucp;
	FArrayBox& vc = *vcp;
	Real* u0ptr = u[0][lev][igrid].dataPtr();
	Real* u1ptr = u[1][lev][igrid].dataPtr();
#if (BL_SPACEDIM == 3)
	FArrayBox *wcp;
	if (jgrid < 0) 
	{
	    wcp = new FArrayBox(cbox);
	    fill_patch(*wcp, wcp->box(), u[2][lev-1], lev_interface[mgc], boundary.velocity(2), 0);
	}
	else 
	{
	    wcp = &u[2][lev-1][jgrid];
	}
	FArrayBox& wc = *wcp;
	Real* u2ptr = u[2][lev][igrid].dataPtr();
#endif
	Box creg = lev_interface[mglev].node_face(iface);
	creg.coarsen(rat).grow(t - IntVect::TheUnitVector());
	Real* sptr = source[lev][igrid].dataPtr();
	if (m_hg_terrain)
	{
	    FORT_HGFDIV_TERRAIN(sptr, DIMLIST(sbox),
		D_DECL(uc.dataPtr(), vc.dataPtr(), wc.dataPtr()),
		DIMLIST(cbox),
		D_DECL(u0ptr, u1ptr, u2ptr), DIMLIST(fbox), DIMLIST(creg),
		D_DECL(rat[0], rat[1], rat[2]), idim, idir);
	}
	else
	{
#if (BL_SPACEDIM == 2)
	    FORT_HGFDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), DIMLIST(cbox),
		u0ptr, u1ptr, DIMLIST(fbox), DIMLIST(creg),
		hx, hy, rat[0], rat[1], idim, idir,
		IsRZ(), mg_domain[mgc].bigEnd(0) + 1);
#else
	    FORT_HGFDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), wc.dataPtr(), DIMLIST(cbox),
		u0ptr, u1ptr, u2ptr, DIMLIST(fbox), DIMLIST(creg),
		&hx, &hy, &hz, rat[0], rat[1], rat[2], idim, idir);
#endif
	}
	if (jgrid < 0) 
	{
	    delete ucp;
	    delete vcp;
#if (BL_SPACEDIM == 3)
	    delete wcp;
#endif
	}
    }
    
#if (BL_SPACEDIM == 3) || (defined HG_TERRAIN)
    
    int ga[level_interface::N_CORNER_GRIDS];
    
#if (BL_SPACEDIM == 3)
    
    // PARALLEL
    for (int iedge = 0; iedge < lev_interface[mglev].nedges(); iedge++) 
    {
	// find a fine grid touching this edge
	int igrid;
	for (int i = 0; i < level_interface::N_EDGE_GRIDS; i++) 
	{
	    igrid = lev_interface[mglev].egrid(iedge, i);
	    if (igrid >= 0)
		break;
	}
	const unsigned int geo = lev_interface[mglev].geo(1, iedge);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].flag(1, iedge) )
	    continue;
	// fine grid on just one side
	const Box& sbox = source[lev][igrid].box();
	Real* sptr = source[lev][igrid].dataPtr();
	Box cbox = lev_interface[mglev].edge(iedge);
	IntVect t = cbox.type();
	cbox.coarsen(rat).grow(t).convert(IntVect::TheCellVector());
	Box fbox = cbox;
	fbox.refine(rat);
	FArrayBox *ucp, *vcp, *wcp;
	int jgrid = find_patch(cbox, u[0][lev-1], 0);
	if (jgrid < 0) 
	{
	    ucp = new FArrayBox(cbox);
	    vcp = new FArrayBox(cbox);
	    wcp = new FArrayBox(cbox);
	    fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
	    fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	    fill_patch(*wcp, wcp->box(), u[2][lev-1], lev_interface[mgc], boundary.velocity(2), 0);
	}
	else 
	{
	    ucp = &u[0][lev-1][jgrid];
	    vcp = &u[1][lev-1][jgrid];
	    wcp = &u[2][lev-1][jgrid];
	    cbox = ucp->box();
	}
	FArrayBox& uc = *ucp;
	FArrayBox& vc = *vcp;
	FArrayBox& wc = *wcp;
	FArrayBox uf(fbox), vf(fbox), wf(fbox);
	fill_patch(uf, uf.box(), u[0][lev], lev_interface[mglev], boundary.velocity(0), 0, 1, iedge);
	fill_patch(vf, vf.box(), u[1][lev], lev_interface[mglev], boundary.velocity(1), 0, 1, iedge);
	fill_patch(wf, wf.box(), u[2][lev], lev_interface[mglev], boundary.velocity(2), 0, 1, iedge);
	Box creg = lev_interface[mglev].node_edge(iedge);
	creg.coarsen(rat).grow(t - IntVect::TheUnitVector());
	lev_interface[mglev].geo_array(ga, 1, iedge);
	if (m_hg_terrain)
	{
	    FORT_HGEDIV_TERRAIN(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), wc.dataPtr(), DIMLIST(cbox),
		uf.dataPtr(), vf.dataPtr(), wf.dataPtr(), DIMLIST(fbox),
		DIMLIST(creg),
		rat[0], rat[1], rat[2],
		t.getVect(), ga);
	}
	else
	{
	    FORT_HGEDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), wc.dataPtr(), DIMLIST(cbox),
		uf.dataPtr(), vf.dataPtr(), wf.dataPtr(), DIMLIST(fbox),
		DIMLIST(creg),
		&hx, &hy, &hz,
		rat[0], rat[1], rat[2],
		t.getVect(), ga);
	}
	if (jgrid < 0) 
	{
	    delete ucp;
	    delete vcp;
	    delete wcp;
	}
	// fill in the grids on the other sides, if any
	const Box& freg = lev_interface[mglev].node_edge(iedge);
	for (int i = 1; i < level_interface::N_EDGE_GRIDS; i++) 
	{
	    jgrid = lev_interface[mglev].egrid(iedge, i);
	    if (jgrid >= 0 && jgrid != igrid)
		internal_copy(source[lev], jgrid, igrid, freg);
	}
    }
    
#endif
    // PARALLEL    
    for (int icor = 0; icor < lev_interface[mglev].ncorners(); icor++) 
    {
	// find a fine grid touching this corner
	int igrid;
	for (int i = 0; i < level_interface::N_CORNER_GRIDS; i++) 
	{
	    igrid = lev_interface[mglev].cgrid(icor, i);
	    if (igrid >= 0)
		break;
	}
	const unsigned int geo = lev_interface[mglev].geo(0, icor);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].flag(0, icor) )
	    continue;
	// fine grid on just one side
	const Box& sbox = source[lev][igrid].box();
	Real* sptr = source[lev][igrid].dataPtr();
	Box cbox = lev_interface[mglev].corner(icor);
	cbox.coarsen(rat).grow(1).convert(IntVect::TheCellVector());
	Box fbox = cbox;
	fbox.refine(rat);
	FArrayBox *ucp, *vcp;
	int jgrid = find_patch(cbox, u[0][lev-1], 0);
	if (jgrid < 0) 
	{
	    ucp = new FArrayBox(cbox);
	    vcp = new FArrayBox(cbox);
	    fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
	    fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	}
	else 
	{
	    ucp = &u[0][lev-1][jgrid];
	    vcp = &u[1][lev-1][jgrid];
	    cbox = ucp->box();
	}
	FArrayBox& uc = *ucp;
	FArrayBox& vc = *vcp;
	FArrayBox uf(fbox), vf(fbox);
	fill_patch(uf, uf.box(), u[0][lev], lev_interface[mglev], boundary.velocity(0), 0, 0, icor);
	fill_patch(vf, vf.box(), u[1][lev], lev_interface[mglev], boundary.velocity(1), 0, 0, icor);
#  if (BL_SPACEDIM == 3)
	FArrayBox *wcp;
	if (jgrid < 0) 
	{
	    wcp = new FArrayBox(cbox);
	    fill_patch(*wcp, wcp->box(), u[2][lev-1], lev_interface[mgc], boundary.velocity(2), 0);
	}
	else 
	{
	    wcp = &u[2][lev-1][jgrid];
	}
	FArrayBox& wc = *wcp;
	FArrayBox  wf(fbox);
	fill_patch(wf, wf.box(), u[2][lev], lev_interface[mglev], boundary.velocity(2), 0, 0, icor);
#  endif
	Box creg = lev_interface[mglev].corner(icor);
	creg.coarsen(rat);
	lev_interface[mglev].geo_array(ga, 0, icor);
	if (m_hg_terrain)
	{
	    FORT_HGCDIV_TERRAIN(sptr, DIMLIST(sbox),
		D_DECL(uc.dataPtr(), vc.dataPtr(), wc.dataPtr()),
		DIMLIST(cbox),
		D_DECL(uf.dataPtr(), vf.dataPtr(), wf.dataPtr()),
		DIMLIST(fbox),
		DIMLIST(creg),
		D_DECL(rat[0], rat[1], rat[2]), ga);
	}
	else
	{
	    FORT_HGCDIV(sptr, DIMLIST(sbox),
		D_DECL(uc.dataPtr(), vc.dataPtr(), wc.dataPtr()),
		DIMLIST(cbox),
		D_DECL(uf.dataPtr(), vf.dataPtr(), wf.dataPtr()),
		DIMLIST(fbox),
		DIMLIST(creg),
		&hx, &hy, &hz,
		D_DECL(rat[0], rat[1], rat[2]), ga);
	}
	if (jgrid < 0) 
	{
	    D_TERM(delete ucp;, delete vcp;, delete wcp;);
	}
	// fill in the grids on the other sides, if any
	const Box& freg = lev_interface[mglev].corner(icor);
	for (int i = 1; i < level_interface::N_CORNER_GRIDS; i++) 
	{
	    jgrid = lev_interface[mglev].cgrid(icor, i);
	    if (jgrid >= 0 && jgrid != igrid)
		internal_copy(source[lev], jgrid, igrid, freg);
	}
    }
    
#else
    // PARALLEL    
    for (int icor = 0; icor < lev_interface[mglev].ncorners(); icor++) 
    {
	// find a fine grid touching this corner
	int igrid;
	for (int i = 0; i < 4; i++) 
	{
	    igrid = lev_interface[mglev].cgrid(icor, i);
	    if (igrid >= 0)
		break;
	}
	const unsigned int geo = lev_interface[mglev].geo(0, icor);
	// reject fine-fine interfaces and those without an interior fine grid
	if (geo == level_interface::ALL || igrid < 0 || lev_interface[mglev].flag(0, icor) )
	    continue;
	else if (geo == level_interface::XL || geo == level_interface::XH || geo == level_interface::YL || geo == level_interface::YH) 
	{
	    // fine grid on two adjacent sides
	    int idim = (geo == level_interface::XL || geo == level_interface::XH) ? 0 : 1;
	    int idir = (geo & level_interface::LL) ? -1 : 1;
	    const Box& sbox = source[lev][igrid].box();
	    Box cbox = lev_interface[mglev].corner(icor);
	    Box fbox = lev_interface[mglev].corner(icor);
	    cbox.grow(1 - idim, rat[1-idim]);
	    fbox.grow(1 - idim, rat[1-idim]);
	    if (idir > 0) 
	    {
		cbox.growLo(idim, rat[idim]);
		fbox.growHi(idim, 1);
	    }
	    else 
	    {
		cbox.growHi(idim, rat[idim]);
		fbox.growLo(idim, 1);
	    }
	    cbox.convert(IntVect::TheCellVector()).coarsen(rat);
	    FArrayBox *ucp, *vcp;
	    int jgrid = find_patch(cbox, u[0][lev-1], 0);
	    if (jgrid < 0) 
	    {
		ucp = new FArrayBox(cbox);
		vcp = new FArrayBox(cbox);
		fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
		fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	    }
	    else 
	    {
		ucp = &u[0][lev-1][jgrid];
		vcp = &u[1][lev-1][jgrid];
		cbox = ucp->box();
	    }
	    FArrayBox& uc = *ucp;
	    FArrayBox& vc = *vcp;
	    fbox.convert(IntVect::TheCellVector());
	    FArrayBox uf(fbox), vf(fbox);
	    fill_patch(uf, uf.box(), u[0][lev], lev_interface[mglev], boundary.velocity(0), 0, 0, icor);
	    fill_patch(vf, vf.box(), u[1][lev], lev_interface[mglev], boundary.velocity(1), 0, 0, icor);
	    Box creg = lev_interface[mglev].corner(icor);
	    creg.coarsen(rat);
	    Real* sptr = source[lev][igrid].dataPtr();
	    FORT_HGFDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), DIMLIST(cbox),
		uf.dataPtr(), vf.dataPtr(), DIMLIST(fbox),
		DIMLIST(creg), hx, hy,
		rat[0], rat[1], idim, idir,
		IsRZ(), mg_domain[mgc].bigEnd(0) + 1);
	    if (jgrid < 0) 
	    {
		delete ucp;
		delete vcp;
	    }
	    // fill in the grids on the other sides, if any
	    const Box& freg = lev_interface[mglev].corner(icor);
	    for (int i = 1; i < 4; i++) 
	    {
		jgrid = lev_interface[mglev].cgrid(icor, i);
		if (jgrid >= 0 && jgrid != igrid)
		    internal_copy(source[lev], jgrid, igrid, freg);
	    }
	}
	else if (geo == level_interface::LL || geo == level_interface::HL || geo == level_interface::LH || geo == level_interface::HH) 
	{
	    // outside corner
	    int idir0 = (geo & level_interface::XL) ? -1 : 1;
	    int idir1 = (geo & level_interface::YL) ? -1 : 1;
	    const Box& sbox = source[lev][igrid].box();
	    Box cbox = lev_interface[mglev].corner(icor);
	    cbox.grow(rat).convert(IntVect::TheCellVector()).coarsen(rat);
	    FArrayBox *ucp, *vcp;
	    int jgrid = find_patch(cbox, u[0][lev-1], 0);
	    if (jgrid < 0) 
	    {
		ucp = new FArrayBox(cbox);
		vcp = new FArrayBox(cbox);
		fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
		fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	    }
	    else 
	    {
		ucp = &u[0][lev-1][jgrid];
		vcp = &u[1][lev-1][jgrid];
		cbox = ucp->box();
	    }
	    FArrayBox& uc = *ucp;
	    FArrayBox& vc = *vcp;
	    const Box& fbox = u[0][lev][igrid].box();
	    Box creg = lev_interface[mglev].corner(icor);
	    creg.coarsen(rat);
	    Real* sptr = source[lev][igrid].dataPtr();
	    Real* u0ptr = u[0][lev][igrid].dataPtr();
	    Real* u1ptr = u[1][lev][igrid].dataPtr();
	    FORT_HGODIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), DIMLIST(cbox),
		u0ptr, u1ptr,
		DIMLIST(fbox), DIMLIST(creg),
		hx, hy, rat[0], rat[1], idir0, idir1, IsRZ());
	    if (jgrid < 0) 
	    {
		delete ucp;
		delete vcp;
	    }
	}
	else if (geo == (level_interface::LL | level_interface::HH) || geo == (level_interface::LH | level_interface::HL)) 
	{
	    // diagonal corner
	    const Box& sbox = source[lev][igrid].box();
	    Box cbox = lev_interface[mglev].corner(icor);
	    cbox.grow(rat).convert(IntVect::TheCellVector()).coarsen(rat);
	    FArrayBox *ucp, *vcp;
	    int jgrid = find_patch(cbox, u[0][lev-1], 0);
	    if (jgrid < 0) 
	    {
		ucp = new FArrayBox(cbox);
		vcp = new FArrayBox(cbox);
		fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
		fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	    }
	    else 
	    {
		ucp = &u[0][lev-1][jgrid];
		vcp = &u[1][lev-1][jgrid];
		cbox = ucp->box();
	    }
	    FArrayBox& uc = *ucp;
	    FArrayBox& vc = *vcp;
	    Box fbox = lev_interface[mglev].corner(icor);
	    fbox.grow(rat).convert(IntVect::TheCellVector());
	    FArrayBox uf(fbox), vf(fbox);
	    fill_patch(uf, uf.box(), u[0][lev], lev_interface[mglev], boundary.velocity(0), 0, 0, icor);
	    fill_patch(vf, vf.box(), u[1][lev], lev_interface[mglev], boundary.velocity(1), 0, 0, icor);
	    int jdir = (geo == (level_interface::LL | level_interface::HH)) ? 1 : -1;
	    Box creg = lev_interface[mglev].corner(icor);
	    creg.coarsen(rat);
	    Real* sptr = source[lev][igrid].dataPtr();
	    FORT_HGDDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), DIMLIST(cbox),
		uf.dataPtr(), vf.dataPtr(), DIMLIST(fbox),
		DIMLIST(creg), hx, hy, rat[0], rat[1], jdir, IsRZ());
	    if (jgrid < 0) 
	    {
		delete ucp;
		delete vcp;
	    }
	    // fill in the grids on the other sides, if any
	    const Box& freg = lev_interface[mglev].corner(icor);
	    for (int i = 1; i < 4; i++) 
	    {
		jgrid = lev_interface[mglev].cgrid(icor, i);
		if (jgrid >= 0 && jgrid != igrid)
		    internal_copy(source[lev], jgrid, igrid, freg);
	    }
	}
	else 
	{
	    // inside corner
	    const Box& sbox = source[lev][igrid].box();
	    Box cbox = lev_interface[mglev].corner(icor);
	    Box fbox = lev_interface[mglev].corner(icor);
	    cbox.grow(rat).convert(IntVect::TheCellVector()).coarsen(rat);
	    fbox.grow(1).convert(IntVect::TheCellVector());
	    if ((geo & level_interface::XL) == level_interface::XL)
		fbox.growHi(0, rat[0]-1);
	    else
		fbox.growLo(0, rat[0]-1);
	    if ((geo & level_interface::YL) == level_interface::YL)
		fbox.growHi(1, rat[1]-1);
	    else
		fbox.growLo(1, rat[1]-1);
	    FArrayBox uf(fbox), vf(fbox);
	    fill_patch(uf, uf.box(), u[0][lev], lev_interface[mglev], boundary.velocity(0), 0, 0, icor);
	    fill_patch(vf, vf.box(), u[1][lev], lev_interface[mglev], boundary.velocity(1), 0, 0, icor);
	    int idir0, idir1;
	    if ((geo & level_interface::XL) == level_interface::XL) 
	    {
		idir0 = -1;
		cbox.growLo(0, -1);
	    }
	    else 
	    {
		idir0 = 1;
		cbox.growHi(0, -1);
	    }
	    if ((geo & level_interface::YL) == level_interface::YL) 
	    {
		idir1 = -1;
		cbox.growLo(1, -1);
	    }
	    else 
	    {
		idir1 = 1;
		cbox.growHi(1, -1);
	    }
	    FArrayBox *ucp, *vcp;
	    int jgrid = find_patch(cbox, u[0][lev-1], 0);
	    if (jgrid < 0) 
	    {
		ucp = new FArrayBox(cbox);
		vcp = new FArrayBox(cbox);
		fill_patch(*ucp, ucp->box(), u[0][lev-1], lev_interface[mgc], boundary.velocity(0), 0);
		fill_patch(*vcp, vcp->box(), u[1][lev-1], lev_interface[mgc], boundary.velocity(1), 0);
	    }
	    else 
	    {
		ucp = &u[0][lev-1][jgrid];
		vcp = &u[1][lev-1][jgrid];
		cbox = ucp->box();
	    }
	    FArrayBox& uc = *ucp;
	    FArrayBox& vc = *vcp;
	    cbox = uc.box();
	    Box creg = lev_interface[mglev].corner(icor);
	    creg.coarsen(rat);
	    Real* sptr = source[lev][igrid].dataPtr();
	    FORT_HGIDIV(sptr, DIMLIST(sbox),
		uc.dataPtr(), vc.dataPtr(), DIMLIST(cbox),
		uf.dataPtr(), vf.dataPtr(), DIMLIST(fbox),
		DIMLIST(creg), hx, hy, rat[0], rat[1], idir0, idir1, IsRZ());
	    if (jgrid < 0) 
	    {
		delete ucp;
		delete vcp;
	    }
	    // fill in the grids on the other sides, if any
	    const Box& freg = lev_interface[mglev].corner(icor);
	    for (int i = 1; i < 4; i++) 
	    {
		jgrid = lev_interface[mglev].cgrid(icor, i);
		if (jgrid >= 0 && jgrid != igrid)
		    internal_copy(source[lev], jgrid, igrid, freg);
	    }
	}
  }
#endif
}

void holy_grail_amr_projector::form_solution_vector(PArray<MultiFab>* u, const PArray<MultiFab>& sigma_in)
{
    assert(u != 0);
    if (u) 
    {
	for (int lev = lev_min; lev <= lev_max; lev++) 
	{
	    int mglev = ml_index[lev];
	    Real hxyz[BL_SPACEDIM] = { D_DECL( h[mglev][0], h[mglev][1], h[mglev][2] ) };
	    for (MultiFabIterator d_mfi(dest[lev]); d_mfi.isValid(); ++d_mfi)
	    {
		const Box& gbox = ml_mesh[lev][d_mfi.index()];
		const Box& dbox = d_mfi->box();
		FArrayBox gp[BL_SPACEDIM];
		for (int i = 0; i < BL_SPACEDIM; i++) 
		{
		    gp[i].resize(gbox);
		}
		if (m_hg_terrain)
		{
		    FORT_HGGRAD_TERRAIN(D_DECL(gp[0].dataPtr(), gp[1].dataPtr(), gp[2].dataPtr()),
			DIMLIST(gbox),
			d_mfi->dataPtr(), DIMLIST(dbox),
			DIMLIST(gbox));
		}
		else
		{
#if (BL_SPACEDIM == 2)
		    FORT_HGGRAD(gp[0].dataPtr(), gp[1].dataPtr(), DIMLIST(gbox),
			d_mfi->dataPtr(), DIMLIST(dbox),
			DIMLIST(gbox), &hxyz[0], &hxyz[1], IsRZ());
#else
		    FORT_HGGRAD(gp[0].dataPtr(), gp[1].dataPtr(), gp[2].dataPtr(),
			DIMLIST(gbox),
			d_mfi->dataPtr(), DIMLIST(dbox),
			DIMLIST(gbox), &hxyz[0], &hxyz[1], &hxyz[2]);
#endif
		}		
		if (!m_hg_terrain)
		{
		    DependentMultiFabIterator s_dmfi(d_mfi, sigma_in[lev]);
		    for (int i = 0; i < BL_SPACEDIM; i++) 
		    {
			gp[i].mult(s_dmfi());
			DependentMultiFabIterator u_dmfi(d_mfi, u[i][lev]);
			u_dmfi->minus(gp[i]);
		    }
		}
		else
		{
		    DependentMultiFabIterator s_dmfi(d_mfi, sigma_in[lev]);
		    FArrayBox cross(gbox);
		    for (int i = 0; i < BL_SPACEDIM; i++) 
		    {
			cross.copy(gp[i]);
			cross.mult(s_dmfi(), i, 0, 1);
			DependentMultiFabIterator u_dmfi(d_mfi, u[i][lev]);
			u_dmfi->minus(cross);
		    }
		    DependentMultiFabIterator ul_dmfi(d_mfi, u[BL_SPACEDIM-1][lev]);
		    for (int i = 0; i < BL_SPACEDIM - 1; i++) 
		    {
			cross.copy(gp[BL_SPACEDIM-1]);
			cross.mult(s_dmfi(), BL_SPACEDIM+i, 0, 1);
			DependentMultiFabIterator u_dmfi(d_mfi, u[i][lev]);
			u_dmfi->plus(cross);
			cross.copy(gp[i]);
			cross.mult(s_dmfi(), BL_SPACEDIM+i, 0, 1);
			ul_dmfi->plus(cross);
		    }
		}
	    }
	}
	
	sync_periodic_interfaces();
	for (int lev = lev_max; lev > lev_min; lev--) 
	{
	    const IntVect& rat = gen_ratio[lev-1];
	    restrict_level(dest[lev-1], dest[lev], rat,
		injection_restrictor_class(), level_interface(), 0);
	    for (int i = 0; i < BL_SPACEDIM; i++) 
	    {
		if (!m_hg_terrain)
		{
		    restrict_level(u[i][lev-1], u[i][lev], rat,
			default_restrictor(), level_interface(), 0);
		}
		else
		{
		    restrict_level(u[i][lev-1], u[i][lev], rat,
			terrain_velocity_restrictor_class(i), level_interface(), 0);
		}
	    }
	}
    }
    else 
    {
	sync_periodic_interfaces();
	for (int lev = lev_max; lev > lev_min; lev--) 
	{
	    restrict_level(dest[lev-1], dest[lev], gen_ratio[lev-1],
		injection_restrictor_class(), level_interface(), 0);
	}
    }
}
