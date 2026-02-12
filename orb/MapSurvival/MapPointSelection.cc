#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <string>
#include <numeric>
#include <cassert>
#include <limits>
#include <array>

// Template
template<int R, int C>
struct Mat {
    std::array<double, R * C> d;
    Mat() { d.fill(0.0); }
    double& operator()(int r, int c)       { return d[r * C + c]; }
    double  operator()(int r, int c) const { return d[r * C + c]; }

    static Mat Identity() {
        static_assert(R == C);
        Mat m; for (int i = 0; i < R; i++) m(i,i) = 1.0; return m;
    }
    static Mat Scaled(double s) {
        static_assert(R == C);
        Mat m; for (int i = 0; i < R; i++) m(i,i) = s; return m;
    }
    Mat operator+(const Mat& o) const {
        Mat r; for (int i = 0; i < R*C; i++) r.d[i] = d[i]+o.d[i]; return r;
    }
    Mat& operator+=(const Mat& o) {
        for (int i = 0; i < R*C; i++) d[i] += o.d[i]; return *this;
    }
    Mat operator*(double s) const {
        Mat r; for (int i = 0; i < R*C; i++) r.d[i] = d[i]*s; return r;
    }
    template<int C2>
    Mat<R,C2> operator*(const Mat<C,C2>& o) const {
        Mat<R,C2> r;
        for (int i = 0; i < R; i++)
            for (int j = 0; j < C2; j++) {
                double s = 0;
                for (int k = 0; k < C; k++) s += (*this)(i,k) * o(k,j);
                r(i,j) = s;
            }
        return r;
    }
    Mat<C,R> T() const {
        Mat<C,R> r;
        for (int i = 0; i < R; i++)
            for (int j = 0; j < C; j++) r(j,i) = (*this)(i,j);
        return r;
    }
};

// Helper typedefs
using M6  = Mat<6,6>;
using M4  = Mat<4,4>;
using M3  = Mat<3,3>;
using M26 = Mat<2,6>;
using M23 = Mat<2,3>;
using M36 = Mat<3,6>;
using V3  = Mat<3,1>;

/// Cholesky decomposition of 6x6 PD matrix
bool Chol6(const M6& A, M6& L) {
    L = M6();
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j <= i; j++) {
            double s = A(i,j);
            for (int k = 0; k < j; k++) s -= L(i,k)*L(j,k);
            if (i == j) {
                if (s <= 1e-15) return false;
                L(i,j) = std::sqrt(s);
            } else {
                L(i,j) = s / L(j,j);
            }
        }
    }
    return true;
}

/// Log-determinant of 6x6 PD matrix via Cholesky
double LogDet6(const M6& A) {
    M6 L;
    if (!Chol6(A, L)) return -std::numeric_limits<double>::infinity();
    double ld = 0;
    for (int i = 0; i < 6; i++) ld += std::log(L(i,i));
    return 2.0 * ld;
}

// Structures
struct KeyFrame {
    int id;
    M4 Tcw;
    M3 Rcw; V3 tcw;
    double fx, fy, cx, cy;
    void Decompose() {
        for (int r=0;r<3;r++) { for (int c=0;c<3;c++) Rcw(r,c)=Tcw(r,c); tcw(r,0)=Tcw(r,3); }
    }
};

struct MapPointData { int id; V3 pos; };

struct Observation { int kf_id, mp_id; double u, v; int octave; };

struct ObsInfo { int kf_id; M6 info; };

struct SlamMap {
    std::vector<KeyFrame> keyframes;
    std::vector<MapPointData> mappoints;
    std::vector<Observation> observations;
    std::unordered_map<int,int> kf_idx, mp_idx;
    std::unordered_map<int,std::vector<int>> mp_obs;

    void Build() {
        kf_idx.clear(); mp_idx.clear(); mp_obs.clear();
        for (int i=0;i<(int)keyframes.size();i++)  { kf_idx[keyframes[i].id]=i; keyframes[i].Decompose(); }
        for (int i=0;i<(int)mappoints.size();i++)   mp_idx[mappoints[i].id]=i;
        for (int i=0;i<(int)observations.size();i++){
            auto it=mp_idx.find(observations[i].mp_id);
            if (it!=mp_idx.end()) mp_obs[it->second].push_back(i);
        }
    }
};

// Jacobian Computation
V3 Transform(const M3& R, const V3& t, const V3& p) {
    V3 r;
    for (int i=0;i<3;i++) { r(i,0)=t(i,0); for (int j=0;j<3;j++) r(i,0)+=R(i,j)*p(j,0); }
    return r;
}

M26 PoseJacobian(const V3& Xc, double fx, double fy) {
    double x=Xc(0,0), y=Xc(1,0), z=Xc(2,0);
    double zi=1.0/z, zi2=zi*zi;

    M23 Jp;
    Jp(0,0)=fx*zi;  Jp(0,1)=0;      Jp(0,2)=-fx*x*zi2;
    Jp(1,0)=0;       Jp(1,1)=fy*zi;  Jp(1,2)=-fy*y*zi2;

    M36 Jx;
    Jx(0,0)=1; Jx(1,1)=1; Jx(2,2)=1;
    Jx(0,3)= 0; Jx(0,4)= z; Jx(0,5)=-y;
    Jx(1,3)=-z; Jx(1,4)= 0; Jx(1,5)= x;
    Jx(2,3)= y; Jx(2,4)=-x; Jx(2,5)= 0;

    return Jp * Jx; //DEBUG 2.2: 2x6
}

// MapPointSelector: Localisation Approximation
class MapPointSelector {
public:
    MapPointSelector(const SlamMap& m, double eps=1e-6, double sig=1.0, double sf=1.2)
        : map_(m), eps_(eps), sig_(sig), sf_(sf) { Precompute(); }

    std::vector<int> SelectLazyGreedy(int budget) {
        int n=(int)map_.mappoints.size();
        if (budget>=n) { std::vector<int> a(n); std::iota(a.begin(),a.end(),0); return a; }
        int nk=(int)map_.keyframes.size();

        // Per-keyframe running info matrix
        std::vector<M6> info(nk, M6::Scaled(eps_));
        std::vector<double> ld(nk);
        for (int j=0;j<nk;j++) ld[j]=LogDet6(info[j]);

        // Max-heap
        using E=std::pair<double,int>;
        std::priority_queue<E> pq;
        // std::cout << "  Initial gains..." << std::flush;  //DEBUG 2.11
        for (int i=0;i<n;i++) pq.push({Gain(i,info,ld), i});
        // std::cout << " ok\n";  //DEBUG 2.11

        std::vector<bool> sel(n,false);
        std::vector<int> res; res.reserve(budget);
        long long rc=0;
        int step=std::max(1,budget/20);

        while ((int)res.size()<budget && !pq.empty()) {
            auto [ub,idx]=pq.top(); pq.pop();
            if (sel[idx]) continue;
            double g=Gain(idx,info,ld); rc++;
            if (!pq.empty() && g<pq.top().first) { pq.push({g,idx}); continue; }

            sel[idx]=true; res.push_back(idx);
            auto it=oi_.find(idx);
            if (it!=oi_.end()) for (auto& o:it->second) {
                int ki=map_.kf_idx.at(o.kf_id);
                info[ki]+=o.info; ld[ki]=LogDet6(info[ki]);
            }
            // if ((int)res.size()%step==0)
            //     std::cout<<"    "<<res.size()<<"/"<<budget<<" (recomp "<<rc<<")\n";  //DEBUG 2.11
        }
        // std::cout<<"  Done. Recomps: "<<rc<<" (brute: "<<(long long)n*budget<<")\n";  //DEBUG 2.11
        return res;
    }

    double Utility(const std::vector<int>& sel) const {
        int nk=(int)map_.keyframes.size();
        std::vector<M6> info(nk, M6::Scaled(eps_));
        for (int i:sel) { auto it=oi_.find(i); if (it==oi_.end()) continue;
            for (auto& o:it->second) info[map_.kf_idx.at(o.kf_id)]+=o.info; }
        double u=0; for (int j=0;j<nk;j++) u+=LogDet6(info[j]); return u;
    }

private:
    const SlamMap& map_;
    double eps_,sig_,sf_;
    std::unordered_map<int,std::vector<ObsInfo>> oi_;

    void Precompute() {
        // std::cout<<"  Precompute info..."<<std::flush;  //DEBUG 2.11
        int ok=0,skip=0;
        for (auto& [mi,ois]:map_.mp_obs) {
            auto& mp=map_.mappoints[mi];
            for (int oi:ois) {
                auto& ob=map_.observations[oi];
                auto ki=map_.kf_idx.find(ob.kf_id);
                if (ki==map_.kf_idx.end()){skip++;continue;}
                auto& kf=map_.keyframes[ki->second];
                V3 Xc=Transform(kf.Rcw,kf.tcw,mp.pos);
                if (Xc(2,0)<0.01){skip++;continue;}
                M26 A=PoseJacobian(Xc,kf.fx,kf.fy);
                double s=sig_*std::pow(sf_,ob.octave), w=1.0/(s*s);
                auto At=A.T();
                M6 inf;
                for (int r=0;r<6;r++) for (int c=0;c<6;c++)
                    inf(r,c)=w*(At(r,0)*A(0,c)+At(r,1)*A(1,c));
                oi_[mi].push_back({ob.kf_id,inf});
                ok++;
            }
        }
        // std::cout<<" ("<<ok<<" valid, "<<skip<<" skip)\n";  //DEBUG 2.11
    }

    double Gain(int mi,const std::vector<M6>& info,const std::vector<double>& ld) const {
        auto it=oi_.find(mi); if (it==oi_.end()) return 0;
        double g=0;
        for (auto& o:it->second) {
            int ki=map_.kf_idx.at(o.kf_id);
            g += LogDet6(info[ki]+o.info) - ld[ki];
        }
        return g;
    }
};

// Input
SlamMap LoadMap(const std::string& fn) {
    SlamMap m; std::ifstream f(fn);
    if (!f) { std::cerr<<"Cannot open "<<fn<<"\n"; exit(1); }
    std::string t; int c;
    f>>t>>c; assert(t=="NUM_KEYFRAMES");
    m.keyframes.resize(c);
    for (int i=0;i<c;i++) {
        auto& k=m.keyframes[i]; k.Tcw=M4::Identity();
        f>>k.id;
        for (int r=0;r<3;r++) for (int cc=0;cc<4;cc++) f>>k.Tcw(r,cc);
        f>>k.fx>>k.fy>>k.cx>>k.cy;
    }
    f>>t>>c; assert(t=="NUM_MAPPOINTS");
    m.mappoints.resize(c);
    for (int i=0;i<c;i++) { auto& p=m.mappoints[i]; f>>p.id>>p.pos(0,0)>>p.pos(1,0)>>p.pos(2,0); }
    f>>t>>c; assert(t=="NUM_OBSERVATIONS");
    m.observations.resize(c);
    for (int i=0;i<c;i++) { auto& o=m.observations[i]; f>>o.kf_id>>o.mp_id>>o.u>>o.v>>o.octave; }
    m.Build(); return m;
}

// Output
void SaveMap(const std::string& fn,const SlamMap& m,const std::vector<int>& sel) {
    std::ofstream f(fn); if(!f){std::cerr<<"Cannot write "<<fn<<"\n";exit(1);}
    f<<std::fixed<<std::setprecision(8);
    std::unordered_set<int> ids; for (int i:sel) ids.insert(m.mappoints[i].id);
    f<<"NUM_KEYFRAMES "<<m.keyframes.size()<<"\n";
    for (auto& k:m.keyframes) {
        f<<k.id; for(int r=0;r<3;r++) for(int c=0;c<4;c++) f<<" "<<k.Tcw(r,c);
        f<<" "<<k.fx<<" "<<k.fy<<" "<<k.cx<<" "<<k.cy<<"\n";
    }
    f<<"NUM_MAPPOINTS "<<sel.size()<<"\n";
    for (int i:sel) { auto& p=m.mappoints[i]; f<<p.id<<" "<<p.pos(0,0)<<" "<<p.pos(1,0)<<" "<<p.pos(2,0)<<"\n"; }
    std::vector<const Observation*> ko;
    for (auto& o:m.observations) if(ids.count(o.mp_id)) ko.push_back(&o);
    f<<"NUM_OBSERVATIONS "<<ko.size()<<"\n";
    for (auto* o:ko) f<<o->kf_id<<" "<<o->mp_id<<" "<<o->u<<" "<<o->v<<" "<<o->octave<<"\n";
}

void SaveIDs(const std::string& fn,const SlamMap& m,const std::vector<int>& sel) {
    std::ofstream f(fn+".ids"); for (int i:sel) f<<m.mappoints[i].id<<"\n";
}

// Synthetic Map Generator
SlamMap GenSynthetic(int nkf=50,int nmp=2000,unsigned seed=42) {
    SlamMap m;
    double fx=500,fy=500,cx=320,cy=240; int W=640,H=480;
    double Rc=5.0,maxd=15.0,mind=0.2;
    std::mt19937 rng(seed);
    std::uniform_real_distribution<> ux(-3,3),uy(-2,2),uz(-3,3);
    std::normal_distribution<> noise(0,1.0);

    m.keyframes.resize(nkf);
    for (int i=0;i<nkf;i++) {
        auto& k=m.keyframes[i];
        k.id=i; k.fx=fx; k.fy=fy; k.cx=cx; k.cy=cy;
        double a=2.0*M_PI*i/nkf;
        double px=Rc*std::cos(a), pz=Rc*std::sin(a);
        double ni=1.0/std::sqrt(px*px+pz*pz);

        double fwx=-px*ni, fwz=-pz*ni;
        double rx=fwz, rz=-fwx;
        double rn=std::sqrt(rx*rx+rz*rz); rx/=rn; rz/=rn;
        double dx=0, dy=fwz*rx-fwx*rz, dz=0;
        
        dx = 0*rz - fwz*0;
        dy = fwz*rx - fwx*rz;
        dz = fwx*0 - 0*rx;

        k.Tcw=M4::Identity();
        k.Tcw(0,0)=rx;  k.Tcw(0,1)=0;  k.Tcw(0,2)=rz;
        k.Tcw(1,0)=dx;  k.Tcw(1,1)=dy; k.Tcw(1,2)=dz;
        k.Tcw(2,0)=fwx; k.Tcw(2,1)=0;  k.Tcw(2,2)=fwz;
        k.Tcw(0,3)=-(rx*px+rz*pz);
        k.Tcw(1,3)=0;
        k.Tcw(2,3)=-(fwx*px+fwz*pz);
        k.Decompose();
    }

    m.mappoints.resize(nmp);
    for (int i=0;i<nmp;i++) {
        m.mappoints[i].id=i;
        m.mappoints[i].pos(0,0)=ux(rng);
        m.mappoints[i].pos(1,0)=uy(rng);
        m.mappoints[i].pos(2,0)=uz(rng);
    }

    for (int j=0;j<nkf;j++) {
        auto& k=m.keyframes[j];
        for (int i=0;i<nmp;i++) {
            V3 Xc=Transform(k.Rcw,k.tcw,m.mappoints[i].pos);
            if (Xc(2,0)<mind||Xc(2,0)>maxd) continue;
            double u=fx*Xc(0,0)/Xc(2,0)+cx, v=fy*Xc(1,0)/Xc(2,0)+cy;
            if (u<10||u>W-10||v<10||v>H-10) continue;
            int oct=std::min(3,(int)(Xc(2,0)/(maxd/4.0)));
            m.observations.push_back({j,i,u+noise(rng),v+noise(rng),oct});
        }
    }
    m.Build(); return m;
}

// Main
struct Cfg {
    std::string input,output="reduced_map.txt";
    int budget=-1; double eps=1e-6,sigma=1.0,sf=1.2;
    bool syn=false; int snk=50,snm=2000; unsigned sns=42;
};

void Help(const char* p) {
    std::cout<<"Map Point Selection (Localisation Approximation)\n\n"
        <<"Usage:\n  "<<p<<" --synthetic --budget N [opts]\n  "
        <<p<<" --input FILE --budget N [opts]\n\n"
        <<"  --budget N      Points to keep (required)\n"
        <<"  --output FILE   Output file (default: reduced_map.txt)\n"
        <<"  --prior EPS     Prior precision (default: 1e-6)\n"
        <<"  --sigma S       Pixel noise (default: 1.0)\n"
        <<"  --scale F       ORB scale factor (default: 1.2)\n"
        <<"  --syn_kf N      Synthetic keyframes (default: 50)\n"
        <<"  --syn_mp N      Synthetic map points (default: 2000)\n"
        <<"  --syn_seed N    RNG seed (default: 42)\n";
}

Cfg Parse(int argc,char** argv) {
    Cfg c;
    for (int i=1;i<argc;i++) {
        std::string a=argv[i];
        if      (a=="--input"&&i+1<argc) c.input=argv[++i];
        else if (a=="--output"&&i+1<argc) c.output=argv[++i];
        else if (a=="--budget"&&i+1<argc) c.budget=std::stoi(argv[++i]);
        else if (a=="--prior"&&i+1<argc) c.eps=std::stod(argv[++i]);
        else if (a=="--sigma"&&i+1<argc) c.sigma=std::stod(argv[++i]);
        else if (a=="--scale"&&i+1<argc) c.sf=std::stod(argv[++i]);
        else if (a=="--synthetic") c.syn=true;
        else if (a=="--syn_kf"&&i+1<argc) c.snk=std::stoi(argv[++i]);
        else if (a=="--syn_mp"&&i+1<argc) c.snm=std::stoi(argv[++i]);
        else if (a=="--syn_seed"&&i+1<argc) c.sns=std::stoul(argv[++i]);
        else if (a=="-h"||a=="--help") { Help(argv[0]); exit(0); }
        else { std::cerr<<"Unknown: "<<a<<"\n"; exit(1); }
    }
    if (c.budget<0) { std::cerr<<"--budget required\n"; exit(1); }
    if (!c.syn&&c.input.empty()) { std::cerr<<"--input or --synthetic needed\n"; exit(1); }
    return c;
}

int main(int argc,char** argv) {
    if (argc<2) { Help(argv[0]); return 1; }
    Cfg cfg=Parse(argc,argv);

    SlamMap map;
    if (cfg.syn) {
        std::cout<<"Generating synthetic ("<<cfg.snk<<" KFs, "<<cfg.snm<<" MPs)\n";
        map=GenSynthetic(cfg.snk,cfg.snm,cfg.sns);
    } else {
        std::cout<<"Loading: "<<cfg.input<<"\n";
        map=LoadMap(cfg.input);
    }

    int N=(int)map.mappoints.size(), K=(int)map.keyframes.size(), O=(int)map.observations.size();
    std::cout<<"Map: "<<K<<" KFs, "<<N<<" MPs, "<<O<<" obs ("
             <<std::fixed<<std::setprecision(1)<<(double)O/N<<" obs/pt)\n";

    int budget=std::min(cfg.budget,N);
    std::cout<<"Budget: "<<budget<<"/"<<N<<" ("<<std::setprecision(1)<<100.0*budget/N<<"%)\n\n";

    MapPointSelector sel(map,cfg.eps,cfg.sigma,cfg.sf);
    std::vector<int> all(N); std::iota(all.begin(),all.end(),0);
    double uf=sel.Utility(all);
    // std::cout<<"  Full utility: "<<std::setprecision(2)<<uf<<"\n\n";

    auto t0=std::chrono::high_resolution_clock::now();
    auto chosen=sel.SelectLazyGreedy(budget);
    auto t1=std::chrono::high_resolution_clock::now();
    double ms=std::chrono::duration<double,std::milli>(t1-t0).count();
    double us=sel.Utility(chosen);

    // Coverage stats
    std::unordered_set<int> cs(chosen.begin(),chosen.end());
    std::vector<int> kk(K,0); int ko=0;
    for (auto& o:map.observations) {
        int mi=map.mp_idx.at(o.mp_id);
        if (cs.count(mi)) { kk[map.kf_idx.at(o.kf_id)]++; ko++; }
    }
    int mn=*std::min_element(kk.begin(),kk.end());
    int mx=*std::max_element(kk.begin(),kk.end());

    std::cout<<"\n=== RESULTS ===\n"
        <<"  Selected:       "<<chosen.size()<<" pts\n"
        <<"  Utility (reduced):  "<<std::setprecision(2)<<us<<"\n"
        <<"  Utility (full): "<<std::setprecision(2)<<uf<<"\n"
        <<"  Retained:       "<<std::setprecision(1)<<100.0*us/uf<<"%\n"
        <<"  Time:           "<<std::setprecision(0)<<ms<<" ms\n";
        // <<"  Obs/KF:         min="<<mn<<" avg="<<std::setprecision(1)<<(double)ko/K<<" max="<<mx<<"\n"  //DEBUG 2.11
        // <<"  Obs kept:       "<<ko<<"/"<<O<<"\n";  //DEBUG 2.11

    std::cout<<"\nSaving: "<<cfg.output<<"\n";
    SaveMap(cfg.output,map,chosen);
    SaveIDs(cfg.output,map,chosen);
    if (cfg.syn) { std::string ff=cfg.output+".full"; SaveMap(ff,map,all); std::cout<<"  Full: "<<ff<<"\n"; }
    std::cout<<"[MPS] Done.\n";
    return 0;
}