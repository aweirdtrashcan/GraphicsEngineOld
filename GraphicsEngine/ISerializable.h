#pragma once

class ISerializable {
public:
	virtual const char* Serialize() const noexcept = 0;
};