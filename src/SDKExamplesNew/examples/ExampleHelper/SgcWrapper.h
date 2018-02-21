#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

#include <fgrab_prototyp.h>


class SgcWrapper{
public:
	SgcWrapper();
	~SgcWrapper();
	
	void throwLastSgcError();


private:
};