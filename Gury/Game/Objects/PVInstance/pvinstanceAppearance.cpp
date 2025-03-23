
#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"

using namespace RBX;

Vector3 RBX::PVInstance::getQuadFaceNormal(RBX::NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3)
{
    return normalize((v0 - v2).cross(v1 - v3).direction()) * random(0.2f, 1);
}

Array<Vector3> PVInstance::getBrickFaceVertices(NormalId face)
{
    Array<Vector3> vertices;
    switch (face)
    {
    case NormalId::Top:
    {
        vertices.append(Vector3(size.x, size.y, -size.z)); /* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, -size.z)); /* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 3 */
        break;
    }
    case NormalId::Bottom:
    {
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */ 

        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 3 */
        break;
    }
    case NormalId::Front:
    {
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 1 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 3 */
        break;
    }
    case NormalId::Back:
    {
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 3 */
        break;
    }
    case NormalId::Left:
    {
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 2 */

        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 2 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 3 */
        break;
    }
    case NormalId::Right:
    {
        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 1 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 3 */
        break;
    }
    }
    for (int i = 0; i < vertices.size(); i++)
    {
        CoordinateFrame coord = getCoordinateFrame();
        vertices[i] = coord.pointToWorldSpace(vertices[i]);
    }
    return vertices;
}

Render::TextureReserve::TexturePositionalInformation RBX::PVInstance::getFaceUv(NormalId face)
{
    Render::TextureReserve::TexturePositionalInformation info{};
    if (brickTexture != -1)
    {
        Vector3 brickSize = size;

        Vector2 facePosition = SurfaceFactory::get()->getFacePositionRelative(face, brickSize);
        Vector2 faceSize = SurfaceFactory::get()->getFaceSizePx(face, brickSize);

        faceSize.y /= 2;

        auto reserve = Render::TextureReserve::get();

        Render::TextureReserve::TexturePositionalInformation texturePosition = reserve->getTexturePosition(brickTexture);
        Vector2 dimensions = reserve->getSuperImageDimensions();

        info.x = (texturePosition.x + facePosition.x) / dimensions.x;
        info.y = (texturePosition.y + facePosition.y) / dimensions.y;
        info.cx = faceSize.x / dimensions.x;
        info.cy = faceSize.y / dimensions.y;

    }
    return info;

}

void RBX::PVInstance::onPrimitivePVChanged(Primitive* primitive)
{
    void* ud = primitive->getUserdata();
    Instance* instance = (Instance*)ud;

    if (instance)
    {
        PVInstance* pvInstance = toInstance<PVInstance>(instance);
        if (pvInstance)
        {
            pvInstance->edit();
        }
    }
}
