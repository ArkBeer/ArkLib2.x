#pragma once
namespace Ark {
	bool LineCollision(const Ark::Line& l1, const Ark::Line& l2) {
		const bool r1 = ((l1.Vertex[1].x - l1.Vertex[0].x)*(l2.Vertex[0].y - l1.Vertex[0].y)
			- (l1.Vertex[1].y - l1.Vertex[0].y)*(l2.Vertex[0].x - l1.Vertex[0].x))
			*((l1.Vertex[1].x - l1.Vertex[0].x)*(l2.Vertex[1].y - l1.Vertex[0].y)
				- (l1.Vertex[1].y - l1.Vertex[0].y)*(l2.Vertex[1].x - l1.Vertex[0].x)) < 0;
		const bool r2 = ((l2.Vertex[1].x - l2.Vertex[0].x)*(l1.Vertex[0].y - l2.Vertex[0].y)
			- (l2.Vertex[1].y - l2.Vertex[0].y)*(l1.Vertex[0].x - l2.Vertex[0].x))
			*((l2.Vertex[1].x - l2.Vertex[0].x)*(l1.Vertex[1].y - l2.Vertex[0].y)
				- (l2.Vertex[1].y - l2.Vertex[0].y)*(l1.Vertex[1].x - l2.Vertex[0].x)) < 0;
		return r1&&r2;
	}
}