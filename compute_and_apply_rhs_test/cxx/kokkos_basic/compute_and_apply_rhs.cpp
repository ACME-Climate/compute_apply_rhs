#include "compute_and_apply_rhs.hpp"

#include "Types.hpp"
#include "Region.hpp"
#include "TestData.hpp"
#include "sphere_operators.hpp"

namespace TinMan
{

void preq_hydrostatic(const ViewUnmanaged<Real[NP][NP]> phis,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> T_v,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> dp,
                      Real Rgas,
                      ViewUnmanaged<Real[NUM_LEV][NP][NP]> phi);

void preq_hydrostatic(const Kokkos::TeamPolicy<>::member_type &team,
                      const ViewUnmanaged<Real[NP][NP]> phis,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> T_v,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                      const ViewUnmanaged<Real[NUM_LEV][NP][NP]> dp,
                      Real Rgas,
                      ViewUnmanaged<Real[NUM_LEV][NP][NP]> phi);

void preq_omega_ps(const Kokkos::TeamPolicy<>::member_type &team,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> vgrad_p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> div_vdp,
                   ViewUnmanaged<Real[NUM_LEV][NP][NP]> omega_p);

void preq_omega_ps(const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> vgrad_p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> div_vdp,
                   ViewUnmanaged<Real[NUM_LEV][NP][NP]> omega_p);

void compute_and_apply_rhs (const TestData& data, Region& region)
{
  using Kokkos::subview;
  using Kokkos::ALL;

  // Create local views
  ViewManaged<Real[NUM_LEV][NP][NP]>    div_vdp("div_vdp");
  ViewManaged<Real[NP][NP]>             Ephi("ephi");
  ViewManaged<Real[NUM_LEV_P][NP][NP]>  eta_dot_dpdn_ie("eta_dot_dpdn");
  ViewManaged<Real[NUM_LEV][2][NP][NP]> grad_p("grad_p");
  ViewManaged<Real[NUM_LEV][NP][NP]>    kappa_star("kappa_star");
  ViewManaged<Real[NUM_LEV][NP][NP]>    omega_p("omega_p");
  ViewManaged<Real[NUM_LEV][NP][NP]>    p("p");
  ViewManaged<Real[NUM_LEV][NP][NP]>    T_v("T_v");
  ViewManaged<Real[NUM_LEV][NP][NP]>    vgrad_p("vgrad_p");
  ViewManaged<Real[NUM_LEV][NP][NP]>    vort("vort");
  ViewManaged<Real[NUM_LEV][2][NP][NP]> vdp("vdp");
  ViewManaged<Real[2][NP][NP]>          grad_tmp("grad_tmp");

  // Input parameters
  const int nets = data.control().nets;
  const int nete = data.control().nete;
  const int n0   = data.control().n0;
  const int np1  = data.control().np1;
  const int nm1  = data.control().nm1;
  const int qn0  = data.control().qn0;
  const Real dt2 = data.control().dt2;

  auto scalars_2d   = region.get_2d_scalars();
  auto tensors_2d   = region.get_2d_tensors();
  auto scalars_3d   = region.get_3d_scalars();
  auto scalars_4d   = region.get_4d_scalars();
  auto Qdp          = region.get_Qdp();
  auto eta_dot_dpdn = region.get_eta_dot_dpdn();

  Kokkos::parallel_for(Kokkos::TeamPolicy<>(nete - nets + 1, Kokkos::AUTO),
                       KOKKOS_LAMBDA(Kokkos::TeamPolicy<>::member_type &team) {
    const int ie = nets + team.league_rank();

    // Serivce arrays
    Real vgrad_T[NP][NP]                   = {};
    Real ttens[NUM_LEV][NP][NP]            = {};
    Real T_vadv[NUM_LEV][NP][NP]           = {};
    Real v_vadv[NUM_LEV][NP][NP][2]        = {};
    Real vtens1[NUM_LEV][NP][NP]           = {};
    Real vtens2[NUM_LEV][NP][NP]           = {};

    // Subviews in the current element
    auto scalars_2d_ie = subview(scalars_2d, ie, ALL(), ALL(), ALL());
    auto tensors_2d_ie = subview(tensors_2d, ie, ALL(), ALL(), ALL(), ALL(), ALL());
    auto scalars_3d_ie = subview(scalars_3d, ie, ALL(), ALL(), ALL(), ALL());
    auto scalars_4d_ie = subview(scalars_4d, ie, ALL(), ALL(), ALL(), ALL(), ALL());

    // Some subviews used more than once
    auto metDet_ie    = subview (scalars_2d_ie, METDET, ALL(), ALL());
    auto spheremp_ie  = subview (scalars_2d_ie, SPHEREMP, ALL(), ALL());
    auto DInv_ie      = subview (tensors_2d_ie, DINV, ALL(), ALL(), ALL(), ALL());
    auto dp3d_ie_n0   = subview (scalars_4d_ie, n0, DP3D, ALL(), ALL(), ALL());
    auto U_ie_n0      = subview (scalars_4d_ie, n0, U, ALL(), ALL(), ALL());
    auto V_ie_n0      = subview (scalars_4d_ie, n0, V, ALL(), ALL(), ALL());
    auto T_ie_n0      = subview (scalars_4d_ie, n0, T, ALL(), ALL(), ALL());

    // Other accessory variables
    Real v1     = 0;
    Real v2     = 0;
    Real Qt     = 0;
    Real glnps1 = 0;
    Real glnps2 = 0;
    Real gpterm = 0;

    if(ie < nete) {
      for (int igp=0; igp<NP; ++igp)
      {
        for (int jgp=0; jgp<NP; ++jgp)
        {
          p(0,igp,jgp) = data.hvcoord().hyai[0]*data.hvcoord().ps0 + 0.5*dp3d_ie_n0(0,igp,jgp);
        }
      }

      Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV - 1), [&](const int ilev) {
        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), KOKKOS_LAMBDA(const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          p(ilev + 1, igp, jgp) = p(ilev, igp, jgp)
                            + 0.5*dp3d_ie_n0(ilev, igp, jgp)
                            + 0.5*dp3d_ie_n0(ilev + 1, igp, jgp);
        });
      });

      team.team_barrier();

      Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV), [&](const int ilev) {
        gradient_sphere(team, subview(p, ilev, ALL(), ALL()), data,
                        DInv_ie, subview(grad_p, ilev, ALL(), ALL(), ALL()));

        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          v1 = U_ie_n0(ilev,igp,jgp);
          v2 = V_ie_n0(ilev,igp,jgp);
          vgrad_p(ilev, igp, jgp) = v1*grad_p(ilev, 0, igp, jgp) + v2 * grad_p(ilev, 1, igp, jgp);

          vdp(ilev, 0, igp, jgp) = v1 * dp3d_ie_n0(ilev, igp, jgp);
          vdp(ilev, 1, igp, jgp) = v2 * dp3d_ie_n0(ilev, igp, jgp);

          scalars_3d_ie(UN0, ilev, igp, jgp) += data.constants().eta_ave_w * vdp(ilev, 0, igp, jgp);
          scalars_3d_ie(VN0, ilev, igp, jgp) += data.constants().eta_ave_w * vdp(ilev, 1, igp, jgp);
        });

        divergence_sphere(team, subview(vdp, ilev, ALL(), ALL(), ALL()), data, metDet_ie, DInv_ie, subview(div_vdp, ilev, ALL(), ALL()));

        vorticity_sphere(team, subview(scalars_3d_ie, UN0, ilev, ALL(), ALL()), subview(scalars_3d_ie, VN0, ilev, ALL(), ALL()), data,
                         metDet_ie, subview (tensors_2d_ie, D, ALL(), ALL(), ALL(), ALL()), subview(vort, ilev, ALL(), ALL()));
      });

      if (qn0==-1)
      {
        Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV), [&](const int ilev) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
            const int igp = idx / NP;
            const int jgp = idx % NP;
            T_v(ilev,igp,jgp) = T_ie_n0(ilev,igp,jgp);
            kappa_star(ilev,igp,jgp) = data.constants().kappa;
          });
        });
      }
      else
      {
        Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV), [&](const int ilev) {
          Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
            const int igp = idx / NP;
            const int jgp = idx % NP;
            Qt = Qdp(ie,qn0,1,ilev,igp,jgp) / dp3d_ie_n0(ilev,igp,jgp);
            T_v(ilev,igp,jgp) = T_ie_n0(ilev,igp,jgp)*(1.0+ (data.constants().Rwater_vapor/data.constants().Rgas - 1.0)*Qt);
            kappa_star(ilev,igp,jgp) = data.constants().kappa;
          });
        });
      }

      team.team_barrier();

      preq_hydrostatic(team, subview(scalars_2d_ie, PHIS, ALL(), ALL()), T_v, p, dp3d_ie_n0, data.constants().Rgas, subview(scalars_3d_ie, PHI, ALL(), ALL(), ALL()));
      preq_omega_ps(p, vgrad_p, div_vdp, omega_p);

      Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV_P), [&](const int ilev) {
        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          eta_dot_dpdn(ie,ilev,igp,jgp)       += data.constants().eta_ave_w * eta_dot_dpdn_ie(ilev,igp,jgp);
          scalars_3d_ie(OMEGA_P,ilev,igp,jgp) += data.constants().eta_ave_w * omega_p(ilev,igp,jgp);
        });
      });

      team.team_barrier();

      Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV_P), [&](const int ilev) {
        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          v1 = U_ie_n0(ilev,igp,jgp);
          v2 = V_ie_n0(ilev,igp,jgp);

          Ephi(igp,jgp) = 0.5 * (v1*v1 + v2*v2) + scalars_3d_ie(PHI,ilev,igp,jgp) + scalars_3d_ie(PECND,ilev,igp,jgp);
        });

        gradient_sphere(team, subview(T_ie_n0, ilev, ALL(), ALL()), data, DInv_ie, grad_tmp);

        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          v1 = U_ie_n0(ilev,igp,jgp);
          v2 = V_ie_n0(ilev,igp,jgp);

          vgrad_T[igp][jgp] = v1*grad_tmp(0,igp,jgp) + v2*grad_tmp(1,igp,jgp);
        });

        gradient_sphere(team, Ephi, data, DInv_ie, grad_tmp);

        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          gpterm = T_v(ilev,igp,jgp) / p(ilev,igp,jgp);

          glnps1 = data.constants().Rgas*gpterm*grad_p(ilev,0,igp,jgp);
          glnps2 = data.constants().Rgas*gpterm*grad_p(ilev,1,igp,jgp);

          v1 = U_ie_n0(ilev,igp,jgp);
          v2 = V_ie_n0(ilev,igp,jgp);

          vtens1[ilev][igp][jgp] = v_vadv[ilev][igp][jgp][0] + v2 * (scalars_2d_ie(FCOR,igp,jgp) + vort(ilev,igp,jgp)) - grad_tmp(0,igp,jgp) - glnps1;
          vtens2[ilev][igp][jgp] = v_vadv[ilev][igp][jgp][1] - v1 * (scalars_2d_ie(FCOR,igp,jgp) + vort(ilev,igp,jgp)) - grad_tmp(0,igp,jgp) - glnps2;

          ttens[ilev][igp][jgp]  = T_vadv[ilev][igp][jgp] - vgrad_T[igp][jgp] + kappa_star(ilev,igp,jgp)*T_v(ilev,igp,jgp)*omega_p(ilev,igp,jgp);
        });
      });

      team.team_barrier();

      Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NUM_LEV), [&](const int ilev) {
        Kokkos::parallel_for(Kokkos::ThreadVectorRange(team, NP * NP), [&](const int idx) {
          const int igp = idx / NP;
          const int jgp = idx % NP;
          scalars_4d_ie(np1, U, ilev, igp, jgp) = spheremp_ie(igp, jgp) * (scalars_4d_ie(nm1, U, ilev, igp, jgp) + dt2 * vtens1[ilev][igp][jgp]);
          scalars_4d_ie(np1, V, ilev, igp, jgp) = spheremp_ie(igp, jgp) * (scalars_4d_ie(nm1, V, ilev, igp, jgp) + dt2 * vtens1[ilev][igp][jgp]);
          scalars_4d_ie(np1, T, ilev, igp, jgp) = spheremp_ie(igp, jgp) * (scalars_4d_ie(nm1, T, ilev, igp, jgp) + dt2 * ttens[ilev][igp][jgp]);
          scalars_4d_ie(np1, DP3D, ilev, igp, jgp) = spheremp_ie(igp, jgp) * (scalars_4d_ie(nm1, DP3D, ilev, igp, jgp) + dt2 * div_vdp(ilev, igp, jgp));
        });
      });
    }
  });
}

void preq_hydrostatic (const Kokkos::TeamPolicy<>::member_type &team,
                       const ViewUnmanaged<Real[NP][NP]> phis,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> T_v,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> dp,
                       Real Rgas,
                       ViewUnmanaged<Real[NUM_LEV][NP][NP]> phi)
{
  Real hkk, hkl;
  Real phii[NUM_LEV][NP][NP];
  Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NP * NP),
                       [&](const int loop_idx) {
    const int jgp = loop_idx / NP;
    const int igp = loop_idx % NP;
    hkk = 0.5 * dp(NUM_LEV-1, igp, jgp) / p(NUM_LEV - 1, igp, jgp);
    hkl = 2.0 * hkk;
    phii[NUM_LEV - 1][igp][jgp] = Rgas * T_v(NUM_LEV - 1, igp, jgp) * hkl;
    phi(NUM_LEV - 1, igp, jgp) = phis(igp, jgp) + Rgas * T_v(NUM_LEV - 1, igp, jgp) * hkk;

    for(int ilev = NUM_LEV - 2; ilev > 1; --ilev) {
      hkk = 0.5 * dp(ilev,igp,jgp) / p(ilev,igp,jgp);
      hkl = 2.0 * hkk;
      phii[ilev][igp][jgp] = phii[ilev + 1][igp][jgp] + Rgas * T_v(ilev, igp, jgp)*hkl;
      phi(ilev, igp, jgp) = phis(igp, jgp) + phii[ilev + 1][igp][jgp] + Rgas*T_v(ilev, igp, jgp)*hkk;
    }

    hkk = 0.5 * dp(0, igp, jgp) / p(0, igp, jgp);
    phi(0, igp, jgp) = phis(igp, jgp) + phii[1][igp][jgp] + Rgas * T_v(0, igp, jgp) * hkk;
  });
  team.team_barrier();
}

void preq_hydrostatic (const ViewUnmanaged<Real[NP][NP]> phis,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> T_v,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                       const ViewUnmanaged<Real[NUM_LEV][NP][NP]> dp,
                       Real Rgas,
                       ViewUnmanaged<Real[NUM_LEV][NP][NP]> phi)
{
  Real hkk, hkl;
  Real phii[NUM_LEV][NP][NP];
  for (int jgp=0; jgp<NP; ++jgp)
  {
    for (int igp=0; igp<NP; ++igp)
    {
      hkk = 0.5 * dp(NUM_LEV-1,igp,jgp) / p(NUM_LEV-1,igp,jgp);
      hkl = 2.0*hkk;
      phii[NUM_LEV-1][igp][jgp] = Rgas*T_v(NUM_LEV-1,igp,jgp)*hkl;
      phi(NUM_LEV-1,igp,jgp) = phis(igp,jgp) + Rgas*T_v(NUM_LEV-1,igp,jgp)*hkk;
    }
    for (int ilev=NUM_LEV-2; ilev>1; --ilev)
    {
      for (int igp=0; igp<NP; ++igp)
      {
        hkk = 0.5 * dp(ilev,igp,jgp) / p(ilev,igp,jgp);
        hkl = 2.0*hkk;
        phii[ilev][igp][jgp] = phii[ilev+1][igp][jgp] + Rgas*T_v(ilev,igp,jgp)*hkl;
        phi(ilev,igp,jgp) = phis(igp,jgp) + phii[ilev+1][igp][jgp] + Rgas*T_v(ilev,igp,jgp)*hkk;
      }
    }
    for (int igp=0; igp<NP; ++igp)
    {
      hkk = 0.5 * dp(0,igp,jgp) / p(0,igp,jgp);
      phi(0,igp,jgp) = phis(igp,jgp) + phii[1][igp][jgp] + Rgas*T_v(0,igp,jgp)*hkk;
    }
  }
}

void preq_omega_ps(const Kokkos::TeamPolicy<>::member_type &team,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> vgrad_p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> div_vdp,
                   ViewUnmanaged<Real[NUM_LEV][NP][NP]> omega_p)
{
  Real ckk, ckl, term;
  Real suml[NP][NP];
  Kokkos::parallel_for(Kokkos::TeamThreadRange(team, NP * NP),
                       [&](const int loop_idx) {
    const int jgp = loop_idx / NP;
    const int igp = loop_idx % NP;

    ckk = 0.5 / p(0, igp, jgp);
    term  = div_vdp(0, igp, jgp);
    omega_p(0, igp, jgp) = vgrad_p(0, igp, jgp) / p(0, igp, jgp) - ckk * term;
    suml[igp][jgp] = term;
    for(int ilev = 1; ilev < NUM_LEV - 1; ++ilev)
    {
      ckk = 0.5 / p(ilev, igp, jgp);
      ckl = 2.0 * ckk;
      term  = div_vdp(ilev, igp, jgp);
      omega_p(ilev, igp, jgp) = vgrad_p(ilev, igp, jgp) / p(ilev, igp, jgp) - ckl * suml[igp][jgp] - ckk * term;

      suml[igp][jgp] += term;
    }

    ckk = 0.5 / p(NUM_LEV - 1, igp, jgp);
    ckl = 2.0 * ckk;
    term = div_vdp(NUM_LEV - 1, igp, jgp);
    omega_p(NUM_LEV - 1, igp, jgp) = vgrad_p(NUM_LEV - 1, igp, jgp) / p(NUM_LEV - 1, igp, jgp) - ckl * suml[igp][jgp] - ckk * term;
  });
  team.team_barrier();
}

void preq_omega_ps(const ViewUnmanaged<Real[NUM_LEV][NP][NP]> p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> vgrad_p,
                   const ViewUnmanaged<Real[NUM_LEV][NP][NP]> div_vdp,
                   ViewUnmanaged<Real[NUM_LEV][NP][NP]> omega_p)
{
  Real ckk, ckl, term;
  Real suml[NP][NP];
  for (int jgp=0; jgp<NP; ++jgp)
  {
    for (int igp=0; igp<NP; ++igp)
    {
      ckk = 0.5 / p(0,igp,jgp);
      term  = div_vdp(0,igp,jgp);
      omega_p(0,igp,jgp) = vgrad_p(0,igp,jgp) / p(0,igp,jgp) - ckk*term;
      suml[igp][jgp] = term;
    }
    for (int ilev=1; ilev<NUM_LEV-1; ++ilev)
    {
      for (int igp=0; igp<NP; ++igp)
      {
        ckk = 0.5 / p(ilev,igp,jgp);
        ckl = 2.0 * ckk;
        term  = div_vdp(ilev,igp,jgp);
        omega_p(ilev,igp,jgp) = vgrad_p(ilev,igp,jgp) / p(ilev,igp,jgp) - ckl*suml[igp][jgp] - ckk*term;

        suml[igp][jgp] += term;
      }
    }
    for (int igp=0; igp<NP; ++igp)
    {
      ckk = 0.5 / p(NUM_LEV-1,igp,jgp);
      ckl = 2.0 * ckk;
      term  = div_vdp(NUM_LEV-1,igp,jgp);
      omega_p(NUM_LEV-1,igp,jgp) = vgrad_p(NUM_LEV-1,igp,jgp) / p(NUM_LEV-1,igp,jgp) - ckl*suml[igp][jgp] - ckk*term;
    }
  }
}

void print_results_2norm (const TestData& data, const Region& region)
{
  // Input parameters
  const int nets = data.control().nets;
  const int nete = data.control().nete;
  const int np1  = data.control().np1;

  auto scalars_4d = region.get_4d_scalars();

  Real vnorm(0.), tnorm(0.), dpnorm(0.);
  for (int ie=nets; ie<nete; ++ie)
  {
    for (int ilev=0; ilev<NUM_LEV; ++ilev)
    {
      for (int igp=0; igp<NP; ++igp)
      {
        for (int jgp=0; jgp<NP; ++jgp)
        {
          vnorm  += std::pow( scalars_4d(ie,np1,U,ilev,igp,jgp)   , 2 );
          vnorm  += std::pow( scalars_4d(ie,np1,V,ilev,igp,jgp)   , 2 );
          tnorm  += std::pow( scalars_4d(ie,np1,T,ilev,igp,jgp)   , 2 );
          dpnorm += std::pow( scalars_4d(ie,np1,DP3D,ilev,igp,jgp), 2 );
        }
      }
    }
  }

  std::cout << "   ---> Norms:\n"
            << "          ||v||_2  = " << std::sqrt (vnorm) << "\n"
            << "          ||T||_2  = " << std::sqrt (tnorm) << "\n"
            << "          ||dp||_2 = " << std::sqrt (dpnorm) << "\n";
}

void dump_results_to_file (const TestData& data, const Region& region)
{
  // Input parameters
  const int nets = data.control().nets;
  const int nete = data.control().nete;
  const int np1  = data.control().np1;

  std::ofstream vxfile, vyfile, tfile, dpfile;
  vxfile.open("elem_state_vx.txt");
  if (!vxfile.is_open())
  {
    std::cout << "Error! Cannot open 'elem_state_vx.txt'.\n";
    std::abort();
  }

  vyfile.open("elem_state_vy.txt");
  if (!vyfile.is_open())
  {
    vxfile.close();
    std::cout << "Error! Cannot open 'elem_state_vy.txt'.\n";
    std::abort();
  }

  tfile.open("elem_state_t.txt");
  if (!tfile.is_open())
  {
    std::cout << "Error! Cannot open 'elem_state_t.txt'.\n";
    vxfile.close();
    vyfile.close();
    std::abort();
  }

  dpfile.open("elem_state_dp3d.txt");
  if (!dpfile.is_open())
  {
    std::cout << "Error! Cannot open 'elem_state_dp3d.txt'.\n";
    vxfile.close();
    vyfile.close();
    tfile.close();
    std::abort();
  }

  vxfile.precision(6);
  vyfile.precision(6);
  tfile.precision(6);
  dpfile.precision(6);

  auto scalars_4d = region.get_4d_scalars();

  for (int ie=nets; ie<nete; ++ie)
  {
    for (int ilev=0; ilev<NUM_LEV; ++ilev)
    {
      vxfile << "[" << ie << ", " << ilev << "]\n";
      vyfile << "[" << ie << ", " << ilev << "]\n";
      tfile  << "[" << ie << ", " << ilev << "]\n";
      dpfile << "[" << ie << ", " << ilev << "]\n";

      for (int igp=0; igp<NP; ++igp)
      {
        for (int jgp=0; jgp<NP; ++jgp)
        {
          vxfile << " " << scalars_4d(ie,U,np1,ilev,igp,jgp)   ;
          vyfile << " " << scalars_4d(ie,V,np1,ilev,igp,jgp)   ;
          tfile  << " " << scalars_4d(ie,T,np1,ilev,igp,jgp)   ;
          dpfile << " " << scalars_4d(ie,DP3D,np1,ilev,igp,jgp);
        }
        vxfile << "\n";
        vyfile << "\n";
        tfile  << "\n";
        dpfile << "\n";
      }
    }
  }

  // Closing files
  vxfile.close();
  vyfile.close();
  tfile.close();
  dpfile.close();
};

} // Namespace TinMan
