#ifndef SUPERANIMSPRITE_H
#define SUPERANIMSPRITE_H

//////////////////////////////////////////////////////////////////////////
#include "scene/resources/texture.h"
#include "core/map.h"
#include "core/vector.h"

typedef struct _ccTex2F {
     float u;
     float v;
} ccTex2F;

typedef struct _ccVertex3F
{
    float x;
    float y;
    float z;
} ccVertex3F;

static inline ccVertex3F vertex3(const float x, const float y, const float z)
{
    ccVertex3F c = {x, y, z};
    return c;
}

typedef struct _ccColor4B
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} ccColor4B;

static inline ccColor4B ccc4(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t o)
{
    ccColor4B c = {r, g, b, o};
    return c;
}

//! a Point with a vertex point, a tex coord point and a color 4B
typedef struct _ccV3F_C4B_T2F
{
    //! vertices (3F)
    ccVertex3F        vertices;            // 12 bytes
//    char __padding__[4];

    //! colors (4B)
    ccColor4B        colors;                // 4 bytes
//    char __padding2__[4];

    // tex coords (2F)
    ccTex2F            texCoords;            // 8 bytes
} ccV3F_C4B_T2F;

//! 4 ccVertex3FTex2FColor4B
typedef struct _ccV3F_C4B_T2F_Quad
{
    //! top left
    ccV3F_C4B_T2F    tl;
    //! bottom left
    ccV3F_C4B_T2F    bl;
    //! top right
    ccV3F_C4B_T2F    tr;
    //! bottom right
    ccV3F_C4B_T2F    br;
} ccV3F_C4B_T2F_Quad;


#include "SuperAnimResource.h"

typedef Texture CCTexture2D;
typedef Rect2 CCRect;

enum AnimState
{
    kAnimStateInvalid = -1,
    kAnimStateInitialized,
    kAnimStatePlaying,
    kAnimStatePause
};

namespace SuperAnim {
    class SuperAnimSprite
    {
    public:
        Ref<Texture> mTexRef;
        CCTexture2D* mTexture;
        ccV3F_C4B_T2F_Quad mQuad;
        String mStringId;
    public:
        SuperAnimSprite();
        SuperAnimSprite(CCTexture2D *theTexture);
        SuperAnimSprite(CCTexture2D *theTexture, CCRect theTextureRect);
        ~SuperAnimSprite();

        void SetTexture(CCTexture2D *theTexture);
        void SetTexture(CCTexture2D *theTexture, CCRect theTextureRect);
    };

    typedef Map<SuperAnimSpriteId, SuperAnimSprite *> IdToSuperAnimSpriteMap;
    class SuperAnimSpriteMgr
    {
        IdToSuperAnimSpriteMap mSuperAnimSpriteMap;
        IdToSuperAnimSpriteMap::Element *mSuperAnimSpriteIterator;
    private:
        SuperAnimSpriteMgr();
        ~SuperAnimSpriteMgr();

    public:
        void dumpSpritesInfo();
        static SuperAnimSpriteMgr *GetInstance();
        static void DestroyInstance();
        SuperAnimSpriteId LoadSuperAnimSprite(String theSpriteName);
        void UnloadSuperSprite(SuperAnimSpriteId theSpriteId);
        SuperAnimSprite * GetSpriteById(SuperAnimSpriteId theSpriteId);
        void BeginIterateSpriteId();
        bool IterateSpriteId(SuperAnimSpriteId &theCurSpriteId);
    };
}

//////////////////////////////////////////////////////////////////////////

#endif // SUPERANIMSPRITE_H
