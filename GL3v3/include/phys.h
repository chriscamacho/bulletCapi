#include <capi.h>
#include <obj.h>


typedef struct __phys_t phys_t;

struct __phys_t {
	Vec		sz;		// "scale" or size of the bodies shape
	void*	body;
};
