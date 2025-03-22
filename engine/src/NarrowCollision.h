#ifndef NARROWCOLLISION_H
#define NARROWCOLLISION_H

#include "Contacts.h"
#include "RigidBody.h"

struct CollisionData
{
    /**
     * Contact array
     */
    Contact *contacts;

    /**
     * Maximum number of contacts the array can hold
     */
    unsigned int contactsLeft;
};

class Primitive
{
public:
    RigidBody *body;
    Matrix4x4f offset;
};

class Sphere : public Primitive
{
public:
    float radius;
    // Position is given by offset of Primitive
};

unsigned int CollisionDetector::sphereAndSphere(
    const CollisionSphere &one,
    const CollisionSphere &two,
    CollisionData *data)
{
    if (data->contactsLeft <= 0)
        return 0;

    Vector3f positionOne = one.getAxis(3);
    Vector3f positionTwo = two.getAxis(3);

    Vector3f midline = positionOne - positionTwo;
    float size = length(midline);

    if (size <= 0.0f || size >= one.radius + two.radius)
    {
        return 0;
    }

    // Normalize the midline vector
    Vector3f normal = midline * (1.0f / size);

    Contact *contact = data->contacts;
    contact->contactNormal = normal;
    contact->contactPoint = positionOne + midline * 0.5f;
    contact->penetration = one.radius + two.radius - size;
    contact->setBodyData(one.body, two.body, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
};

/**
 * Because planes are almost always associated with
 * immovable geometry rather than rigid body, the
 * rigid-body pointer in the Primitive class will typically
 * be NULL.
 */
class Plane : public Primitive
{
public:
    Vector3f normal;
    float offset;
};

unsigned int CollisionDetector::sphereAndHalfSpace(
    const CollisionSphere &sphere,
    const CollisionPlane &plane,
    CollisionData *data)
{
    if (data->contactsLeft <= 0)
        return 0;

    Vector3f position = sphere.getAxis(3);

    float ballDistance = dot(plane.normal, position) - sphere.radius - plane.offset;

    if (ballDistance >= 0)
        return 0;

    Contact *contact = data->contacts;
    contact->contactNormal = plane.normal;
    contact->penetration = -ballDistance;
    contact->contactPoint = position - plane.normal * (ballDistance + sphere.radius);
    contact->setBodyData(sphere.body, NULL, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}

class Box : public Primitive
{
public:
    Vector3f halfSize;
};

/**
 * Note from book:
 *
 * Despite the marginal theoretical advantage of such an algorithm, I have found
 * them to have no efficiency gain in practice. Generating and testing a vertex is so fast
 * that additional checking has a marginal effect. If you are familiar with optimizing
 * for SIMD hardware, you will also notice that this algorithm lends itself very easily to
 * parallel implementation, which makes the alternatives even less attractive.
 */
unsigned int CollisionDetector::boxAndHalfSpace(
    const CollisionBox &box,
    const CollisionPlane &plane,
    CollisionData *data)
{
    if (data->contactsLeft <= 0)
        return 0;

    if (!IntersectionTests::boxAndHalfSpace(box, plane))
        return 0;

    static float vertices[8][3] = {
        { 1,  1,  1},
        {-1,  1,  1},
        { 1, -1,  1},
        {-1, -1,  1},
        { 1,  1, -1},
        {-1,  1, -1},
        { 1, -1, -1},
        {-1, -1, -1}};

    Contact *contact = data->contacts;
    unsigned int contactsUsed = 0;

    for (int i = 0; i < 8; i++)
    {
        Vector3f vertexPos(vertices[i][0], vertices[i][1], vertices[i][2]);
        vertexPos.x *= box.halfSize.x;
        vertexPos.y *= box.halfSize.y;
        vertexPos.z *= box.halfSize.z;

        vertexPos = box.transform * vertexPos;

        float vertexDistance = dot(vertexPos, plane.normal);

        if (vertexDistance <= plane.offset)
        {
            contact->contactPoint = plane.direction;
            contact->contactPoint *= (vertexDistance - plane.offset);
            contact->contactPoint += vertexPos;
            contact->contactNormal = plane.normal;
            contact->penetration = plane.offset - vertexDistance;

            contact->setBodyData(box.body, NULL, data->friction, data->restitution);

            contact++;
            contactsUsed++;
            if (contactsUsed == data->contactsLeft)
                return contactsUsed;
        }
    }

    data->addContacts(contactsUsed);
    return contactsUsed;
}

unsigned int CollisionDetector::boxAndSphere(
    const CollisionBox &box,
    const CollisionSphere &sphere,
    CollisionData *data)
{
    if (data->contactsLeft <= 0)
        return 0;

    Vector3f sphereCenter = sphere.getAxis(3);
    Vector3f sphereRelativeCenter = inverse(box.transform) * sphereCenter;

    if (abs(sphereRelativeCenter.x) - sphere.radius > box.halfSize.x ||
        abs(sphereRelativeCenter.y) - sphere.radius > box.halfSize.y ||
        abs(sphereRelativeCenter.z) - sphere.radius > box.halfSize.z)
    {
        return 0;
    }

    Vector3f closestPoint(0, 0, 0);
    float distance;

    // clang-format off
    distance = sphereRelativeCenter.x;
    if (distance > box.halfSize.x) distance = box.halfSize.x;
    if (distance < -box.halfSize.x) distance = -box.halfSize.x;
    closestPoint.x = distance;

    distance = sphereRelativeCenter.y;
    if (distance > box.halfSize.y) distance = box.halfSize.y;
    if (distance < -box.halfSize.y) distance = -box.halfSize.y;
    closestPoint.y = distance;

    distance = sphereRelativeCenter.z;
    if (distance > box.halfSize.z) distance = box.halfSize.z;
    if (distance < -box.halfSize.z) distance = -box.halfSize.z;
    closestPoint.z = distance;
    // clang-format on

    /**
     * This code deviates from the one in the textbook
     * in that the length is not squared to eliminate the
     * square root operation when calculating the
     * penetration depth.
     */

    distance = length(closestPoint - sphereRelativeCenter);
    if (distance * distance > sphere.radius * sphere.radius)
        return 0;

    Vector3f closestPointWorld = box.transform * closestPoint;

    Contact *contact = data->contacts;
    contact->contactNormal = normalize(closestPointWorld - sphereCenter);
    contact->contactPoint = closestPointWorld;
    contact->penetration = sphere.radius - distance;
    contact->setBodyData(box.body, sphere.body, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}

/**
 * @param toCenter The vector from the center of the first box to the center of the second.
 */
static inline float penetrationOnAxis(
    const CollisionBox &one,
    const CollisionBox &two,
    const Vector3f &axis,
    const Vector3f &toCenter)
{
    float oneProject = transformToAxis(one, axis);
    float twoProject = transformToAxis(two, axis);

    float distance = fabsf(dot(toCenter, axis));

    return oneProject + twoProject - distance;
}

#define TEST_OVERLAP(axis) penetrationOnAxis(one, two, (axis), toCenter)

bool IntersectionTests::boxAndBox(
    const CollisionBox &one,
    const CollisionBox &two)
{
    Vector3f toCenter = two.getAxis(3) - one.getAxis(3);

    return (
        TEST_OVERLAP(one.getAxis(0)) &&
        TEST_OVERLAP(one.getAxis(1)) &&
        TEST_OVERLAP(one.getAxis(2)) &&

        TEST_OVERLAP(two.getAxis(0)) &&
        TEST_OVERLAP(two.getAxis(1)) &&
        TEST_OVERLAP(two.getAxis(2)) &&

        TEST_OVERLAP(cross(one.getAxis(0), two.getAxis(0))) &&
        TEST_OVERLAP(cross(one.getAxis(0), two.getAxis(1))) &&
        TEST_OVERLAP(cross(one.getAxis(0), two.getAxis(2))) &&
        TEST_OVERLAP(cross(one.getAxis(1), two.getAxis(0))) &&
        TEST_OVERLAP(cross(one.getAxis(1), two.getAxis(1))) &&
        TEST_OVERLAP(cross(one.getAxis(1), two.getAxis(2))) &&
        TEST_OVERLAP(cross(one.getAxis(2), two.getAxis(0))) &&
        TEST_OVERLAP(cross(one.getAxis(2), two.getAxis(1))) &&
        TEST_OVERLAP(cross(one.getAxis(2), two.getAxis(2))));
}

#undef TEST_OVERLAP

/**
 * For now, ignore implementing complete box-box collision detection.
 */

#endif