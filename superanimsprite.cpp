#include "SuperAnimSprite.h"
#include "core/os/os.h"

namespace SuperAnim {

SuperAnimSpriteId LoadSuperAnimSprite(String theSpriteName){
    return SuperAnimSpriteMgr::GetInstance()->LoadSuperAnimSprite(theSpriteName);
}

/*----------------------- SuperAnimSpriteMgr----------------------------*/
SuperAnimSpriteId SuperAnimSpriteMgr::LoadSuperAnimSprite(String theSpriteName){
    print_line("+SuperAnimSpriteMgr::LoadSuperAnimSprite(" + theSpriteName + ")");
    //see if we already have the sprite loaded.
    for(IdToSuperAnimSpriteMap::Element *ptr = mSuperAnimSpriteMap.front(); ptr != NULL; ptr = ptr->next()){
        SuperAnimSprite *sprPtr = ptr->value();
        if(sprPtr->mStringId == theSpriteName){
            print_line("-SuperAnimSpriteMgr::LoadSuperAnimSprite.. .existing instance");
            return ptr->key();
        }
    }

    print_line("loading the texture using ResourceLoader");
    //load the texture resource. typecast the pointer to void*
    Ref<Texture> texRef =  ResourceLoader::load(theSpriteName);
    if(texRef.is_null()){
        print_line("texRef is Null!!");
        return NULL;
    }
    SuperAnimSpriteId sprId = texRef.ptr();

    //create the SuperAnim Sprite instance.
    SuperAnimSprite *spritePtr = memnew(SuperAnimSprite);
    //initialize it with the texture instance.
    spritePtr->SetTexture(static_cast<CCTexture2D*>(sprId));
    spritePtr->mTexRef = texRef; //saving the texture Ref so we ensure that this stays in mem.
    spritePtr->mStringId = theSpriteName;

    //set the map with the value.
    mSuperAnimSpriteMap[spritePtr] = spritePtr;

    OS::get_singleton()->print("-SuperAnimSpriteMgr::LoadSuperAnimSprite(..... new instance) %p", spritePtr);
    return spritePtr;
}

static SuperAnimSpriteMgr * sInstance = NULL;

void SuperAnimSpriteMgr::dumpSpritesInfo()
{
    print_line("------------------------");
    for(IdToSuperAnimSpriteMap::Element *ptr = mSuperAnimSpriteMap.front();
        ptr != NULL; ptr  = ptr->next()){
        SuperAnimSprite *spritePtr = ptr->value();

        ccV3F_C4B_T2F_Quad &aOriginQuad = spritePtr->mQuad;
        OS::get_singleton()->print("Pointer: %p\n", spritePtr);
        OS::get_singleton()->print("Point: (%f, %f, %f)\n", aOriginQuad.tl.texCoords.u,  aOriginQuad.tl.texCoords.v, aOriginQuad.tl.vertices.z);
        OS::get_singleton()->print("Point: (%f, %f, %f)\n", aOriginQuad.bl.texCoords.u,  aOriginQuad.bl.texCoords.v, aOriginQuad.bl.vertices.z);
        OS::get_singleton()->print("Point: (%f, %f, %f)\n", aOriginQuad.br.texCoords.u,  aOriginQuad.br.texCoords.v, aOriginQuad.br.vertices.z);
        OS::get_singleton()->print("Point: (%f, %f, %f)\n", aOriginQuad.tr.texCoords.u,  aOriginQuad.tr.texCoords.v, aOriginQuad.tr.vertices.z);
    }
    print_line("----------End--------------");
}

SuperAnimSpriteMgr::SuperAnimSpriteMgr()
{

}

SuperAnimSpriteMgr::~SuperAnimSpriteMgr()
{
    for(IdToSuperAnimSpriteMap::Element *ptr = mSuperAnimSpriteMap.front(); ptr != NULL; ptr = ptr->next()){
        memfree(ptr->value());
    }
//    for (IdToSuperAnimSpriteMap::iterator anItr = mSuperAnimSpriteMap.begin();
//         anItr != mSuperAnimSpriteMap.end(); ++anItr)
//    {
//        delete anItr->second;
//    }

    mSuperAnimSpriteMap.clear();
}

SuperAnimSpriteMgr *SuperAnimSpriteMgr::GetInstance()
{
    if (sInstance == NULL)
    {
        sInstance = new SuperAnimSpriteMgr();
    }

    return sInstance;
}

void SuperAnimSpriteMgr::DestroyInstance()
{
    if (sInstance)
    {
        delete sInstance;
        sInstance = NULL;
    }
}

void SuperAnimSpriteMgr::BeginIterateSpriteId(){
    mSuperAnimSpriteIterator = mSuperAnimSpriteMap.front();
}

bool SuperAnimSpriteMgr::IterateSpriteId(SuperAnimSpriteId &theCurSpriteId){
    if (mSuperAnimSpriteIterator == NULL) {
        theCurSpriteId = InvalidSuperAnimSpriteId;
        return false;
    }

    theCurSpriteId = mSuperAnimSpriteIterator->value();
//    mSuperAnimSpriteIterator++;
    mSuperAnimSpriteIterator = mSuperAnimSpriteIterator->next();

    return true;
}

SuperAnimSprite * SuperAnimSpriteMgr::GetSpriteById(SuperAnimSpriteId theSpriteId)
{
    IdToSuperAnimSpriteMap::Element *anItr = mSuperAnimSpriteMap.find(theSpriteId);
    if (anItr != NULL)
    {
        return anItr->value();
    }

    return NULL;
}
/*------------------------------ END -----------------------------*/

SuperAnimSprite::SuperAnimSprite()
{
    mTexture = NULL;
    memset(&mQuad, 0, sizeof(mQuad));
}

SuperAnimSprite::SuperAnimSprite(CCTexture2D *theTexture)
{
    mTexture = NULL;
    memset(&mQuad, 0, sizeof(mQuad));
    SetTexture(theTexture);
}

SuperAnimSprite::SuperAnimSprite(CCTexture2D *theTexture, CCRect theTextureRect)
{
    mTexture = NULL;
    memset(&mQuad, 0, sizeof(mQuad));
    SetTexture(theTexture, theTextureRect);
}

SuperAnimSprite::~SuperAnimSprite()
{
    if (mTexture != NULL)
    {
        //mTexture->release();
        mTexture = NULL;
    }
}

void SuperAnimSprite::SetTexture(CCTexture2D *theTexture)
{
    CCRect aRect;
    aRect.pos = Vector2(0, 0);
    aRect.size = theTexture->get_size();
    SetTexture(theTexture, aRect);
}

void SuperAnimSprite::SetTexture(CCTexture2D *theTexture, CCRect theTextureRect)
{
    print_line("+SuperAnimSprite::SetTexture()");
    if (theTexture == NULL)
    {
        print_line("-SuperAnimSprite::SetTexture(Null texture)");
        return;
    }

    // retain this texture in case removed by removeUnusedTextures();
    mTexture = theTexture;

    // Set Texture coordinates
    CCRect theTexturePixelRect = theTextureRect;//CC_RECT_POINTS_TO_PIXELS(theTextureRect);

    float aTextureWidth = (float)mTexture->get_width(); //mTexture->getPixelsWide();
    float aTextureHeight = (float)mTexture->get_height(); //mTexture->getPixelsHigh();

    float aLeft, aRight, aTop, aBottom;
    aLeft = theTexturePixelRect.pos.x / aTextureWidth;
    aRight = (theTexturePixelRect.pos.x + theTexturePixelRect.size.width) / aTextureWidth;
    aTop = (theTexturePixelRect.pos.y + theTexturePixelRect.size.height) / aTextureHeight;
    aBottom = theTexturePixelRect.pos.y / aTextureHeight;

    mQuad.tl.texCoords.u = aLeft;
    mQuad.tl.texCoords.v = aTop;
    mQuad.bl.texCoords.u = aLeft;
    mQuad.bl.texCoords.v = aBottom;
    mQuad.br.texCoords.u = aRight;
    mQuad.br.texCoords.v = aBottom;
    mQuad.tr.texCoords.u = aRight;
    mQuad.tr.texCoords.v = aTop;

    // Set position
    //float x1 = 0;
    //float y1 = 0;
    //float x2 = x1 + theTextureRect.size.width;
    //float y2 = y1 + theTextureRect.size.height;

    float x1 = theTexturePixelRect.size.width * -0.5f;
    float y1 = theTexturePixelRect.size.height * -0.5f;
    float x2 = theTexturePixelRect.size.width * 0.5f;
    float y2 = theTexturePixelRect.size.height * 0.5f;

    mQuad.bl.vertices = vertex3(x1, y1, 0);
    mQuad.br.vertices = vertex3(x2, y1, 0);
    mQuad.tl.vertices = vertex3(x1, y2, 0);
    mQuad.tr.vertices = vertex3(x2, y2, 0);

    // Set color
    ccColor4B aDefaultColor = {255, 255, 255, 255};
    mQuad.bl.colors = aDefaultColor;
    mQuad.br.colors = aDefaultColor;
    mQuad.tl.colors = aDefaultColor;
    mQuad.tr.colors = aDefaultColor;
//    OS::get_singleton()->print("Point: (%f, %f, %f)\n", mQuad.bl.vertices.x,  mQuad.bl.vertices.y, mQuad.bl.vertices.z);
//    OS::get_singleton()->print("Point: (%f, %f, %f)\n", mQuad.br.vertices.x,  mQuad.br.vertices.y, mQuad.br.vertices.z);
//    OS::get_singleton()->print("Point: (%f, %f, %f)\n", mQuad.tl.vertices.x,  mQuad.tl.vertices.y, mQuad.tl.vertices.z);
//    OS::get_singleton()->print("Point: (%f, %f, %f)\n", mQuad.tr.vertices.x,  mQuad.tr.vertices.y, mQuad.tr.vertices.z);
    print_line("-SuperAnimSprite::SetTexture()");
}

}
