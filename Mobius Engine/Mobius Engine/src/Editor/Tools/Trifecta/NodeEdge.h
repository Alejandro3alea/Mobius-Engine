#pragma once

struct NodeEdge
{
	NodeEdge(const int _from = -1, const int _to = -1) : from(_from), to(_to) {}

	int from, to;
};