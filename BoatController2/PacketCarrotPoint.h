#ifndef PACKET_CARROT_POINT_H_
#define PACKET_CARROT_POINT_H_
#include "Serializable.h"
class PacketCarrotPoint:public Serializable{
public:
	PacketCarrotPoint(const Point2D& carrot) :_carrot(carrot) {
		
	}

	Point2D _carrot;
private:
	const char* getTypeField() const {
		return "CP";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_carrot._x, 3));
		loader.load(String(_carrot._y, 3));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(20);
		if (!unloader.unload(field))return false;
		_carrot._x = field.toFloat();
		if (!unloader.unload(field))return false;
		_carrot._y = field.toFloat();
		return true;
	}
};
#endif