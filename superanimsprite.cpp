#include "SuperAnimSprite.h"

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

    //load the texture resource. typecast the pointer to void*
    SuperAnimSpriteId sprId =  ResourceLoader::load(theSpriteName).ptr();

    //create the SuperAnim Sprite instance.
    SuperAnimSprite *spritePtr = memnew(SuperAnimSprite);
    //initialize it with the texture instance.
    spritePtr->SetTexture(static_cast<CCTexture2D*>(sprId));
    spritePtr->mStringId = theSpriteName;

    //set the map with the value.
    mSuperAnimSpriteMap[sprId] = spritePtr;

    print_line("-SuperAnimSpriteMgr::LoadSuperAnimSprite(..... new instance)");
    return sprId;
}

static SuperAnimSpriteMgr * sInstance = NULL;
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
    aTop = theTexturePixelRect.pos.y / aTextureHeight;
    aBottom = (theTexturePixelRect.pos.y + theTexturePixelRect.size.height) / aTextureHeight;

    mQuad.bl.texCoords.u = aLeft;
    mQuad.bl.texCoords.v = aBottom;
    mQuad.br.texCoords.u = aRight;
    mQuad.br.texCoords.v = aBottom;
    mQuad.tl.texCoords.u = aLeft;
    mQuad.tl.texCoords.v = aTop;
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

    print_line("-SuperAnimSprite::SetTexture()");
}

}
