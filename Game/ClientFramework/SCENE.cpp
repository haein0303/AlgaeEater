

#include "Util.h"
#include "DxEngine.h"
#include "SCENE.h"

SCENE::SCENE()
{
}

SCENE::SCENE(SceneTag tag, DxEngine* dxEngine):_tag(tag),_dxengine(dxEngine)
{
}
