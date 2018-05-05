#ifndef PACKET_WAY_POINTS_H_
#define PACKET_WAY_POINTS_H_
#include "Serializable.h"
#include "WayPoints.h"

class PacketWayPoints:public Serializable{
public:
	PacketWayPoints(const WayPoints& way_points):_way_points(way_points){}

	WayPoints _way_points;
private:
	const char* getTypeField() const {
		return "WPS";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		size_t n = _way_points.size();
		loader.load(String(n));
		for(size_t i=0;i<n;i++){
			loader.load(String(_way_points[i]._x));
			loader.load(String(_way_points[i]._y));
		}
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(20);
		size_t n;
		if (!unloader.unload(field)) return false;
		n = field.toInt();
		_way_points.clear();
		Point2D wp;
		for(size_t i=0;i<n;i++){
			if (!unloader.unload(field)) return false;
			wp._x = field.toFloat();
			if (!unloader.unload(field)) return false;
			wp._y = field.toFloat();
			_way_points.addWayPoint(wp);
		}
		return true;
	}

};
#endif