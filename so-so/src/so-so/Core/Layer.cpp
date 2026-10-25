#include "Layer.h"

#include <utility>

soso::Layer::Layer(std::string debugName)
	: m_DebugName(std::move(debugName)) {

}
