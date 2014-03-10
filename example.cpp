#include <assert.h>
#include <random>
#include "topo_a.h"
#include "position.h"
#include "vec.h"
#include "container.h"
#include "topology.h"
#include "object_container.h"
#include "bresenham.h"
#include "distance_field.h"


//struct vec2{ int x; int y; vec2(int x_, int y_) :x(x_), y(y_){} };
#include <ostream>
#include <stdio.h>
#include <iostream>


int main(int argc, char** argv)
{
	return 0;
}
//
//dsif::ivec remove_facing(griddle::position_facing<dsif::position<ivec>, griddle::sym8> pos) { return pos.pos; }
//griddle::position_facing<position<ivec>, griddle::sym8> add_facing(ivec pos) { return{ pos, griddle::facing::NORTH_8 }; }
//
//int main(int argc, char** argv)
//{
//	typedef ivec vec;
//	typedef position<vec> pvec;
//	const aligned_grid ag(5, 5);
//	const cartesian_topology ctag(ALL_DIRECTIONS);
//	const bounded_grid bg(4, 8, 8, 12);
//	const cartesian_topology ctbg(ALL_DIRECTIONS);
//	const torus t(5, 5);
//	const torus_topology ctt(ALL_DIRECTIONS, t);
//	const klein_bottle kb(7, 7, true);
//	const torus_topology ctkb(ALL_DIRECTIONS, kb);
//	const real_projective_plane rpp(9, 9);
//	const torus_topology ctrpp(ALL_DIRECTIONS, rpp);
//	const sphere s(15, 15);
//	const torus_topology cts(ALL_DIRECTIONS, s);
//	const moebius_strip ms(4, 10,true);
//	const torus_topology ctms(ALL_DIRECTIONS, ms);
//	const ring rn(3, 8, false);
//	const torus_topology ctrn(ALL_DIRECTIONS, rn);
//
//	const std::vector<const chart<vec>*> ucv{
//		(chart<vec>*)&ag, (chart<vec>*)&bg, (chart<vec>*)&t, (chart<vec>*)&kb,
//		(chart<vec>*) &rpp, (chart<vec>*)&s, (chart<vec>*)&ms, (chart<vec>*)&rn };
//	const union_chart<vec> uc(ucv);
//	const std::vector<const step_topology<vec>*> uct{
//		(step_topology<vec>*)&ctag, (step_topology<vec>*)&ctbg, (step_topology<vec>*)&ctt, (step_topology<vec>*)&ctkb,
//		(step_topology<vec>*) &ctrpp, (step_topology<vec>*)&cts, (step_topology<vec>*)&ctms, (step_topology<vec>*)&ctrn };
//
//	const union_topology<vec> everywhere(uct, ALL_DIRECTIONS);
//	typedef union_position<vec> uvec;
//	typedef position<uvec> puvec;
//	portal_topology<uvec> anywhere(everywhere,ALL_DIRECTIONS);
//
//	anywhere.add_two_way_portal({ { 4, 4 }, 0 }, NORTH, { { 14, 14 }, 5 }, SOUTH);
//	//for (const auto& p : anywhere.build_neighbourhood(uvec({ 0, 0 }, 0)))
//	//{
//	//	return p.pos.pos.pos.y;
//	//}
//
//	cross_pile_container<uvec, locateable<uvec>> cpc(uc,5);
//	typedef locateable<uvec> loc_t;
//	loc_t* L = new loc_t();
//	typedef std::shared_ptr<loc_t> s_loc;
//	s_loc sl(L);
//	cpc.claim(sl);
//	cpc.place(sl, { { 4, 4 }, 0 });
//	cpc.move(sl, { { 5, 5 }, 5 });
//	//return sl->get_location().pos.pos.pos.y;
//	int return_me = INT_MIN;
//	
//
//	//return return_me;
//	const range<uvec> uctr(uc);
//
//	//griddle::distance_field<uvec, size_t> paths(anywhere,uc);
//	//paths.zero();
//	//paths.initialise({ { 1, 2 }, 0 });
//	//paths.update();
//	//return paths.get_distances()(uvec{ { 3, 3 }, 5});
//
//	griddle::dir4 my_dir(3);
//	griddle::sym4 my_transform(1, true);
//	griddle::sym4 my_other_transform(3, true);
//	my_dir = my_dir + my_transform*my_other_transform;
//
//	//const aligned_grid big_grid(20, 20);
//	//const griddle::cartesian_topology<8> grid_topo(griddle::ALL_DELTAS);
//
//	const aligned_grid big_sphere(5, 5);
//	const griddle::sphere_topology sphere_topo(5, 5);
//
//	//return sphere_topo.neighbour({ { 0, 0 } ,griddle::facing::NORTH_8}, griddle::walking::LEFT_8).pos.x;
//	//const griddle::transforming_topology<vec, griddle::dir8, vec, griddle::sym8> boring_sphere(sphere_topo,griddle::ALL_DELTAS);
//
//	griddle::portal_topology<vec, griddle::dir8, vec, griddle::sym8> weird_grid(big_sphere, sphere_topo, griddle::ALL_DELTAS, griddle::facing::SOUTH_8);
//	typedef griddle::position_facing<position<vec>, griddle::sym8> vecf;
//	//weird_grid.add_bidirectional_portal({ 0, 0 }, griddle::walking::FORWARD_8, { { 19, 19 }, griddle::facing::EAST_FLIPPED_8 });
//	//return weird_grid.neighbour({ { 0, 0 }, griddle::facing::NORTH_8 }, griddle::walking::FORWARD_8).face.rotation;
//
//	//griddle::bresenham_pather_8<vec> bres_flat;
//	//bres_flat.follow_line(grid_topo, { 0, 0 }, 5, 5, [&return_me](vec pos) ->bool {return_me = pos.y; return true; });
//
//	griddle::bresenham_pather_8<vecf> bres;
//	bres.follow_line(weird_grid, { { 0, 0 }, griddle::facing::NORTH_8 }, 0, -5,
//		[&return_me](vecf pos)->bool
//	{
//		return_me = pos.pos.x;
//		return true;
//	});
//
//
//	griddle::distance_field<vec, griddle::dir8,vecf> paths(weird_grid,big_sphere);
//	paths.zero();
//	paths.initialise({ { 1, 2 }, 0 });
//	paths.update(add_facing, remove_facing);
//	//return paths.get_distances()({ 15, 13 });
//	const auto big_sphere_range = range<vec>(big_sphere);
//	container<vec, char> chars(big_sphere);
//	container<vec, char> ons(big_sphere);
//	for (const auto& p : big_sphere_range) chars(p) = 'a' + (5*p.y + p.x);
//	for (const auto& p : big_sphere_range) ons(p) = (rand()%40 < (p.x+20) ? 1 : 1);
//	char view[35][35];
//	vecf look_pos;
//	vecf my_pos{ { 0, 0 }, griddle::facing::NORTH_8 };
//	vecf my_new_pos{ { 0, 0 }, griddle::facing::NORTH_8 };
//	while (true)
//	{
//		for (int y = -17; y < 18; y++)
//		for (int x = -17; x < 18; x++)
//		{
//
//			bres.follow_line(weird_grid, my_pos, x, y,
//				[&look_pos](vecf pos)->bool
//			{
//				look_pos = pos;
//				return true;
//			});
//			view[y + 17][x + 17] = (big_sphere.in_range(look_pos.pos) && ons(look_pos.pos)) ? chars(look_pos.pos) : ' ';
//		}
//		view[17][17] = '@';
//		for (int y = -17; y < 18; y++)
//		{
//			for (int x = -17; x < 18; x++)
//			{
//				std::cout << view[y + 17][x + 17];
//			}
//			std::cout << '\n';
//		}
//		std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;
//
//		char c;
//		c = getchar();
//		//std::cin >> c;
//		switch (c)
//		{
//		case 'q':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::FORWARD_LEFT_8);
//			break;
//		case 'w':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::FORWARD_8);
//			break;
//		case 'e':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::FORWARD_RIGHT_8);
//			break;
//		case 'a':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::LEFT_8);
//			break;
//		case 'd':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::RIGHT_8);
//			break;
//		case 'z':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::BACK_LEFT_8);
//			break;
//		case 'x':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::BACK_8);
//			break;
//		case 'c':
//			my_new_pos = weird_grid.neighbour(my_pos, griddle::walking::BACK_RIGHT_8);
//			break;
//		}
//		if (big_sphere.in_range(my_new_pos.pos))
//			my_pos = my_new_pos;
//	}
//
//
//
//	//return (griddle::walking::FORWARD_4 + griddle::facing::REFLECT_NE_4) == griddle::walking::RIGHT_4;
//
//	//return my_dir.dir;
//
//	//typedef pposition<ivec> iivec;
//	//iivec ii{ 5, 6 };
//	//typedef union_pposition<ivec> uuivec;
//	//uuivec uui{ { 8, 8 }, 8 };
//	////uuivec uui{ { 7, 6 }, 3 };
//	//typedef pposition<uuivec> puuivec;
//	//puuivec puui{ { 5, 9 }, 1 };
//
//	//pposition<ivec> pp = { 4, 4 };
//
//	//return (*(uctr.begin() + 400)).get_component();
//
//
//	//typedef map_topo_base<std::pair<int,int>> torus4;
//	//typedef cross_topo_base<torus4, torus4> torus44;
//	//typedef cross_topo_base<torus44, torus4> torus444;
//	//torus4 torus1({
//	//	{ { 0, 0 }, 0 },
//	//	{ { 0, 1 }, 1 },
//	//	{ { 1, 0 }, 2 },
//	//	{ { 1, 1 }, 3 }
//	//});
//	//torus44 cross_torus(torus1, torus1);
//	//torus444 cross_cross_torus(cross_torus, torus1);
//	//concrete_topo<int, torus444> cct_(cross_cross_torus, 4);
//	//int tsum = 0;
//	//for (auto it : cct_)
//	//{
//	//	tsum += it;
//	//}
//	//return tsum;
//
//	//typedef func_topo_base<vec2> grid100;
//	//typedef cross_topo_base<grid100,grid100> grid100100;
//	//typedef cross_topo_base<grid100100, grid100> grid100100100;
//
//
//
//	//grid100 grid1([](const vec2& it) { return 10 * it.y + it.x; }, 100);
//	//grid100100 cross_grid(grid1, grid1);
//	//grid100100100 cross_cross_grid(cross_grid, grid1);
//
//	//typedef cross_topo_base<torus4, grid100> torusgrid;
//	//torusgrid torg(torus1, grid1);
//	//concrete_topo<float, torusgrid> tg(torg,0.3f);
//	//float fsum = 0.f;
//	//for (auto it : tg)
//	//{
//	//	fsum += it;
//	//}
//	//return (int)fsum;
//
//	//concrete_topo<int, grid100100100> ccg_(cross_cross_grid, 4);
//	//auto cg_ = ccg_({ 4, 4 });
//	//int sum = 0;
//	//for (auto it : cg_)
//	//{
//	//	sum += it;
//	//}
//	//auto g_ = cg_({ 1, 2 });
//	//sum = 0;
//	//for (auto it : g_)
//	//{
//	//	sum += it;
//	//}
//	//return sum;
//	////typedef func_topo_base<vec2> grid100;
//	////typedef cross_topo_base<vec2, vec2> grid100100;
//	////typedef cross_topo_base<grid100100::index, vec2> grid100100100;
//	////grid100 grid1([](const vec2& it) { return 10 * it.y + it.x; },100);
//	////grid100100 cross_grid(grid1,grid1);
//	////grid100100100 cross_cross_grid(cross_grid, grid1);
//	////concrete_topo<int, grid100100100> ccg_(cross_cross_grid,4);
//	////auto cg_ = ccg_({ 1, 1 });
//	////auto ccgg_ = cg_({ 2, 3 });
//	////int sum = 0;
//	////for (auto it : cg_)
//	////{
//	////	sum += it;
//	////}
//	////return sum;
//	////return cg_({ { 1, 3 }, { 5, 4 } });
//	////concrete_cross_topo<int, grid100100::index,grid100::index> ccg(cross_cross_grid, 5);
//	////auto cg = ccg({ 1, 1 });
//	////return cg({ { 1, 4 }, { 6, 5 } });
//	////int sum = 0;
//	////for (auto it : ccg)
//	////{
//	////	sum += it;
//	////}
//	////return sum;
//	////return ccg({ { { 1, 1 }, { 1, 1 } }, { 1, 1 } });
//	//return cross_cross_grid.getIndex({ { { 1, 1 }, { 1, 1 } }, { 1, 1 } });
//	////grid_topo<bool, 10, 10> g(false);
//	//return 0;
//}
//
//
//
////#include "grid_topo.h"
//
////#include "layer3.h"
////#include "grid2d.h"
//
//
//
////#include "grid.h"
////#include "layer.h"
////#include "layer2.h"
////
////template<typename T>
////class topo : public grid<T, 10, 10>{};
////
////int main(int argc, char** argv)
////{
////	grid1<int, 10, 10> g1;
////	int count = 0;
////	for (auto& i : g1)
////	{
////		i = count++;
////	}
////	//return g1({ 7, 7 });
////
////	const grid2<int, 10, 10,10> g2;
////	count = 0;
////	for (auto i : g2)
////	{
////		count += (i + 1);
////	}
////	return g2({ 7, 7 ,5});
////	//layer<topo<bool> > walls;
////	//layer<topo<int> > dijkstra;
////	//iter it{ 6, 6 };
////	//walls(it) = true;
////	//dijkstra(it) = 5;
////	//walls({ 4, 5 }) = true;
////	//function_layer<topo<bool> > not_walls([&](topo<bool>::iterator index) -> bool {return !walls(index); });
////	//return (int)not_walls({ 4, 5 });
////
////	//layer<>;
////	//layer<bool, grid<bool, 10, 10>,grid<bool,10,10>::coordinate> walls;
////	//walls(grid<bool,bool,10>::coordinate(4,4)) = true;
////
////}