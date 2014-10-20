//
//  HelloWorldScene.m
//  ChessKing
//
//  Created by echo on 2014. 10. 14..
//  Copyright echo 2014년. All rights reserved.
//
// -----------------------------------------------------------------------

#import "HelloWorldScene.h"
#import "IntroScene.h"

CGFloat dX[6] = {-1, 0, 1, 1, 0, -1,};
CGFloat dY[6] = {-1, -1, 0, 1, 1, 0,};


CGPoint operator+(const CGPoint &p1, const CGPoint &p2)
{
    CGPoint sum = { p1.x + p2.x, p1.y + p2.y };
    return sum;
}

CGPoint operator*(const CGPoint &p1, const float &f)
{
    CGPoint sum = { p1.x * f, p1.y * f };
    return sum;
}

CGPoint operator/(const CGPoint &p1, const float &f)
{
    CGPoint sum = { p1.x / f, p1.y / f };
    return sum;
}

CGPoint operator-(const CGPoint &p1, const CGPoint &p2)
{
    CGPoint sum = { p1.x - p2.x, p1.y - p2.y };
    return sum;
}
CGPoint turnCGPointByRadian(const CGPoint&p1, const float radian)
{
    return ccp(p1.x * cosf(radian) - p1.y * sinf(radian), p1.x * sinf(radian) + p1.y * cosf(radian));
}



bool isInsidePolygon(CGPoint *polygon,int N,CGPoint p)
{
    int counter = 0;
    int i;
    double xinters;
    CGPoint p1,p2;
    
    p1 = polygon[0];
    for (i=1;i<=N;i++) {
        p2 = polygon[i % N];
        if (p.y > MIN(p1.y,p2.y)) {
            if (p.y <= MAX(p1.y,p2.y)) {
                if (p.x <= MAX(p1.x,p2.x)) {
                    if (p1.y != p2.y) {
                        xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
                        if (p1.x == p2.x || p.x <= xinters)
                            counter++;
                    }
                }
            }
        }
        p1 = p2;
    }
    
    if (counter % 2 == 0)
        return(false);
    else
        return(true);
}

CGPoint getCenterPosition(int x, int y)
{
    CGPoint centerPoint;
    centerPoint.x = (1.f * powf(3.f, 0.5f) * MAP_RADIUS * cosf(M_PI / 6.f) * x);
    centerPoint.y = ( powf(3.f , 0.5f) * MAP_RADIUS * y) - (powf(3.f, 0.5f) * MAP_RADIUS * sinf(M_PI / 6.f) * x) ;
    return centerPoint;
}

CGPoint getCenterPositionWithHexaPoint(CGPoint hexaPoint)
{
    return getCenterPosition(hexaPoint.x, hexaPoint.y);
}

CGPoint getHexaPointByScreenPoint(CGPoint point)
{
    for(int x = 0 ;  x < MAP_WIDTH; x++)
    {
        for(int y = 0 ; y < MAP_HEIGHT; y++)
        {
            CGPoint centerPoint = getCenterPosition(x, y);
            CGPoint verts[6];
            for( int i = 0 ; i < 6; i++)
            {
                verts[i] = ccp(centerPoint.x + cosf(M_PI / 3.f * i) * MAP_RADIUS,
                               centerPoint.y + sinf(M_PI / 3.f * i) * MAP_RADIUS );
            }
            if(isInsidePolygon(verts, 6, point))
                return ccp(x,y);
        }
    }
    return ccp(-1,-1);
}

bool isMovalbeAction(CGPoint startHexaPoint, int velocity, CGPoint wannaGoHexaPoint)
{
    
    for( int i = 0 ; i < 6; i++) // index for Angle
    {
        for( int v = 1 ; v <= velocity; v++)
        {
            CGPoint point = ccp( startHexaPoint.x + dX[i] * v,
                                startHexaPoint.y + dY[i] * v);
            if(CGPointEqualToPoint(wannaGoHexaPoint, point))
                return true;
        }
    }
    return false;
}
// -----------------------------------------------------------------------
#pragma mark - HelloWorldScene
// -----------------------------------------------------------------------

@implementation HelloWorldScene
{
    CCSprite *_sprite;
}

// -----------------------------------------------------------------------
#pragma mark - Create & Destroy
// -----------------------------------------------------------------------

+ (HelloWorldScene *)scene
{
    return [[self alloc] init];
}

// -----------------------------------------------------------------------

- (id)init
{
    
    // Apple recommend assigning self with supers return value
    self = [super init];
    if (!self) return(nil);
    unitList = [NSMutableArray array];
    mapNodeList = [NSMutableArray array];
    moveableAreaList = [NSMutableArray array];
    animationCount = 0;
    
    // Enable touch handling on scene node
    self.userInteractionEnabled = YES;
    
    // Create a colored background (Dark Grey)
    CCNodeColor *background = [CCNodeColor nodeWithColor:[CCColor colorWithRed:0.2f green:0.2f blue:0.2f alpha:1.0f]];
    [self addChild:background];
    
    // Add a sprite
    _sprite = [CCSprite spriteWithImageNamed:@"Icon-72.png"];
    _sprite.position  = ccp(self.contentSize.width/2,self.contentSize.height/2);
    //[self addChild:_sprite];
    
    // Animate sprite with action
    CCActionRotateBy* actionSpin = [CCActionRotateBy actionWithDuration:1.5f angle:360];
    [_sprite runAction:[CCActionRepeatForever actionWithAction:actionSpin]];
    
    // Create a back button
    //    CCButton *backButton = [CCButton buttonWithTitle:@"[ Menu ]" fontName:@"Verdana-Bold" fontSize:18.0f];
    //    backButton.positionType = CCPositionTypeNormalized;
    //    backButton.position = ccp(0.85f, 0.95f); // Top Right of screen
    //    [backButton setTarget:self selector:@selector(onBackClicked:)];
    //    [self addChild:backButton];
    
    
    
    
    [self generateMapByHardCoding];
    [self initVerts];
    [self initMapBorder];
    [self generateUnitByHardCoding];
    
    whoTurn = PLAYER_1;
    
    
    
    movableFocusArea = [[CCDrawNode alloc]init];
    [self addChild:movableFocusArea];
    
    nowMoveNode = [[CCDrawNode alloc]init];
    [self addChild:nowMoveNode];
    
    // done
	return self;
}


// -----------------------------------------------------------------------

- (void)dealloc
{
    // clean up code goes here
}

// -----------------------------------------------------------------------
#pragma mark - Enter & Exit
// -----------------------------------------------------------------------

- (void)onEnter
{
    // always call super onEnter first
    [super onEnter];
    
    // In pre-v3, touch enable and scheduleUpdate was called here
    // In v3, touch is enabled by setting userInteractionEnabled for the individual nodes
    // Per frame update is automatically enabled, if update is overridden
    
}

// -----------------------------------------------------------------------

- (void)onExit
{
    // always call super onExit last
    [super onExit];
}

// -----------------------------------------------------------------------
#pragma mark - Touch Handler
// -----------------------------------------------------------------------

-(void) touchBegan:(UITouch *)touch withEvent:(UIEvent *)event {
    CGPoint touchLoc = [touch locationInNode:self];
    
    
    CGPoint touchedHexaPoint = getHexaPointByScreenPoint(touchLoc);
    // Log touch location
    
    CCLOG(@"%@ / %@",NSStringFromCGPoint(touchLoc),NSStringFromCGPoint(touchedHexaPoint));
    
    
    if(isPicked == true && pickedUnit != nil)
    {
        pickedUnit.position = getCenterPositionWithHexaPoint(pickedUnit.hexaPosition);
    }
    pickedUnit = nil;
    isPicked = false;
    
    for (Unit* unit in unitList) {
        if(CGPointEqualToPoint(unit.hexaPosition, touchedHexaPoint)
           && unit.owner == whoTurn)
        {
            isPicked = true;
            pickedUnit = unit;
        }
    }
    
    if(isPicked == true && pickedUnit != nil)
    {
        pickedUnit.position = ccp( (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * 9.f + touchLoc.x) / 10.f,
                                  (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * 9.f + touchLoc.y) / 10.f);
        [self showMovableArea:touchedHexaPoint velocity:pickedUnit.velocity];
    }
    /*
     // Move our sprite to touch location
     CCActionMoveTo *actionMove = [CCActionMoveTo actionWithDuration:1.0f position:touchLoc];
     [_sprite runAction:actionMove];
     */
}

-(void)touchMoved:(UITouch *)touch withEvent:(UIEvent *)event
{
    CGPoint touchLoc = [touch locationInNode:self];
    
    if(isPicked == true && pickedUnit != nil)
    {
        pickedUnit.position = ccp( (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * 9.f + touchLoc.x) / 10.f,
                                  (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * 9.f + touchLoc.y) / 10.f);
        nowMovePoint = ccp( getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * 2.5 - touchLoc.x * 1.5 ,
                           getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * 2.5 - touchLoc.y * 1.5);
        if(CGPointEqualToPoint(getHexaPointByScreenPoint(touchLoc), pickedUnit.hexaPosition) )
            ;
        else
            [self showNowMoveNode];
    }
}

-(void)touchEnded:(UITouch *)touch withEvent:(UIEvent *)event
{
    CGPoint touchLoc = [touch locationInNode:self];
    
    if(isPicked == true && pickedUnit != nil)
    {
        if(CGPointEqualToPoint(getHexaPointByScreenPoint(touchLoc), pickedUnit.hexaPosition) )
            ;
        else
        {
            if (pickedUnit.unitType == HORSE) {
                [self horeseJump:pickedUnit ToHexa:getHexaPointByScreenPoint(nowMovePoint)];
            }
            else
            {
                [self onMoveWithVectorType:[self getVectorTypeByFrom:pickedUnit.hexaPosition To:getHexaPointByScreenPoint(nowMovePoint)]
                                    Length:[self getVectorSizeFrom:pickedUnit.hexaPosition To:getHexaPointByScreenPoint(nowMovePoint)]
                                    Target:pickedUnit isFirstMove:YES];
                
            }
            if(whoTurn == PLAYER_1)
                whoTurn = PLAYER_2;
            else whoTurn = PLAYER_1;
        }
    }
    [self clearMovablePosition];
    [self clearNowMoveNode];
}



- (void)showMovableArea:(CGPoint)startHexaPoint velocity:(int)velocity
{
    [self clearMovablePosition];
    CGFloat dX[6] = {-1, 0, 1, 1, 0, -1,};
    CGFloat dY[6] = {-1, -1, 0, 1, 1, 0,};
    
    for( int i = 0 ; i < 6; i++) // index for Angle
    {
        for( int v = 1 ; v <= velocity; v++)
        {
            CGPoint point = ccp( startHexaPoint.x + dX[i] * v,
                                startHexaPoint.y + dY[i] * v);
            
            if([self isEnablePosition:point] == NO)
            {
                break;
            }
            Unit* unit = [self getUnitInHexaPoint:point];
            if(unit != nil)
            {
                if(unit.owner == pickedUnit.owner)
                {
                    if(pickedUnit.unitType == HORSE)
                    {
                        [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:ccc4(30, 128, 30, 196)]];
                        [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                        continue;
                    }
                    else
                    {
                        [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:ccc4(30, 128, 30, 196)]];
                        [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                        break;
                    }
                }
                else
                {
                    [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:ccc4(255, 30, 30, 196)]];
                    [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                    break;
                }
            }
            else
            {
                [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:ccc4(128, 30, 30, 196)]];
                [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
            }
        }
    }
}
- (void)clearMovablePosition
{
    [movableFocusArea clear];
    [moveableAreaList removeAllObjects];
    
}


-(void)generateMapByHardCoding
{
    int maxY = 10;
    int maxX = 7;
    int startX = 2;
    int startY = 2;
    for( int x = 0 ; x < maxX; x ++)
    {
        for( int y = 0 ; y < maxY - (x%2) ; y++)
        {
            CGPoint point =  ccp(startX + x,
                                 startY + y + ((x+1)/2));
            [mapNodeList addObject:[NSValue valueWithCGPoint:point]];
        }
    }
}

-(void)initVerts
{
    for(int x = 0 ; x < MAP_WIDTH; x++)
    {
        for( int y= 0 ; y < MAP_HEIGHT; y++)
        {
            CGPoint centerPoint = getCenterPosition(x, y);
            for( int i = 0 ; i < 6; i++)
            {
                verts[x][y][i] = ccp(centerPoint.x + cosf(M_PI / 3.f * i) * MAP_RADIUS,
                                     centerPoint.y + sinf(M_PI / 3.f * i) * MAP_RADIUS );
            }
        }
    }
}
-(void)initMapBorder
{
    mapBorder = [[CCDrawNode alloc]init];
    [self addChild:mapBorder];
    for (NSValue* value in mapNodeList) {
        CGPoint centerPoint = [value CGPointValue];
        
        // Draw Hexagon
        [mapBorder drawPolyWithVerts:verts[(int)centerPoint.x][(int)centerPoint.y] count:6 fillColor:[CCColor colorWithCcColor4f:ccc4f(0.f, 0.f, 0.f, 0.f)] borderWidth:1.f borderColor:[CCColor colorWithCcColor4f:ccc4f(0.f, 1.f, 0.f, 1.f)] ];
        
#ifdef HEXA_INDEX_SHOW
        // Label for Coord
        CCLabelTTF* label = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%d,%d",(int)centerPoint.x,(int)centerPoint.y] fontName:@"Verdana"fontSize:20.f];
        label.position = getCenterPositionWithHexaPoint(centerPoint);
        [self addChild:label];
#endif
    }
    
}

-(bool)isEnablePosition:(CGPoint)hexaPoint
{
    for (NSValue *value in mapNodeList) {
        CGPoint point = [value CGPointValue];
        if(CGPointEqualToPoint(point, hexaPoint))
            return true;
    }
    return false;
}
-(bool)isEnableMovePosition:(CGPoint)hexaPoint
{
    for(NSValue *value in moveableAreaList)
    {
        CGPoint point = [value CGPointValue];
        if( CGPointEqualToPoint(hexaPoint, point))
        {
            return true && [self isEnablePosition:hexaPoint];
        }
    }
    return false;
}

-(void)generateUnitByHardCoding
{
    
    //Player_1
    {
        Unit* pawn1 = [Unit UnitWithName:@"P" unitType:PAWN HP:5 Damage:1 Velocity:2 weight:0 owner:PLAYER_1 HexaPosition:ccp(4,6)];
        [self addChild:pawn1];
        [unitList addObject:pawn1];
        
        Unit* pawn2 = [Unit UnitWithName:@"P" unitType:PAWN HP:5 Damage:1 Velocity:2 weight:0 owner:PLAYER_1 HexaPosition:ccp(6,7)];
        [self addChild:pawn2];
        [unitList addObject:pawn2];
        
        Unit* horse1 = [Unit UnitWithName:@"H" unitType:HORSE HP:7 Damage:2 Velocity:4 weight:0 owner:PLAYER_1 HexaPosition:ccp(4,5)];
        [self addChild:horse1];
        [unitList addObject:horse1];
        
        Unit* horse2 = [Unit UnitWithName:@"H" unitType:HORSE HP:7 Damage:2 Velocity:4 weight:0 owner:PLAYER_1 HexaPosition:ccp(6,6)];
        [self addChild:horse2];
        [unitList addObject:horse2];
        
        Unit* bishop = [Unit UnitWithName:@"B" unitType:BISHOP HP:6 Damage:3 Velocity:3 weight:1 owner:PLAYER_1 HexaPosition:ccp(5,6)];
        [self addChild:bishop];
        [unitList addObject:bishop];
        
        Unit* king = [Unit UnitWithName:@"K" unitType:KING HP:6 Damage:3 Velocity:3 weight:1 owner:PLAYER_1 HexaPosition:ccp(5,5)];
        [self addChild:king];
        [unitList addObject:king];
    }
    
    
    //Player_2
    {
        Unit* pawn1 = [Unit UnitWithName:@"P" unitType:PAWN HP:5 Damage:1 Velocity:2 weight:0 owner:PLAYER_2 HexaPosition:ccp(4,9)];
        [self addChild:pawn1];
        [unitList addObject:pawn1];
        
        Unit* pawn2 = [Unit UnitWithName:@"P" unitType:PAWN HP:5 Damage:1 Velocity:2 weight:0 owner:PLAYER_2 HexaPosition:ccp(6,10)];
        [self addChild:pawn2];
        [unitList addObject:pawn2];
        
        Unit* horse1 = [Unit UnitWithName:@"H" unitType:HORSE HP:7 Damage:2 Velocity:4 weight:0 owner:PLAYER_2 HexaPosition:ccp(4,10)];
        [self addChild:horse1];
        [unitList addObject:horse1];
        
        Unit* horse2 = [Unit UnitWithName:@"H" unitType:HORSE HP:7 Damage:2 Velocity:4 weight:0 owner:PLAYER_2 HexaPosition:ccp(6,11)];
        [self addChild:horse2];
        [unitList addObject:horse2];
        
        Unit* bishop = [Unit UnitWithName:@"B" unitType:BISHOP HP:6 Damage:3 Velocity:3 weight:1 owner:PLAYER_2 HexaPosition:ccp(5,10)];
        [self addChild:bishop];
        [unitList addObject:bishop];
        
        Unit* king = [Unit UnitWithName:@"K" unitType:KING HP:6 Damage:3 Velocity:3 weight:1 owner:PLAYER_2 HexaPosition:ccp(5,11)];
        [self addChild:king];
        [unitList addObject:king];
    }
    
    for (Unit* unit in unitList) {
        [unit setPosition:getCenterPositionWithHexaPoint(unit.hexaPosition)];
    }
    
}
-(Unit*)getUnitInHexaPoint:(CGPoint)hexaPoint
{
    for (Unit* unit in unitList) {
        if(CGPointEqualToPoint(unit.hexaPosition, hexaPoint))
            return unit;
    }
    return nil;
}


- (void)showNowMoveNode
{
    [self clearNowMoveNode];
    CGPoint moveHexaPoint = getHexaPointByScreenPoint(nowMovePoint);
    if([self isEnableMovePosition:moveHexaPoint])
    {
        [nowMoveNode drawDot:getCenterPositionWithHexaPoint(getHexaPointByScreenPoint(nowMovePoint)) radius:MOVABLE_RAIDUS*1.5f color:[CCColor colorWithCcColor3b:ccc3(255, 0, 0)]];
    }
    else
    {
        //Find Near Point
        float min = INFINITY;
        CGPoint nearHexaPoint;
        for(NSValue *value in moveableAreaList)
        {
            CGPoint point = [value CGPointValue];
            if( min > powf((point.x - moveHexaPoint.x), 2) + powf((point.y - moveHexaPoint.y), 2) )
            {
                min = powf((point.x - moveHexaPoint.x), 2) + powf((point.y - moveHexaPoint.y), 2);
                nearHexaPoint = point;
            }
        }
        [nowMoveNode drawDot:getCenterPositionWithHexaPoint(nearHexaPoint) radius:MOVABLE_RAIDUS*1.5f color:[CCColor colorWithCcColor3b:ccc3(255, 0, 0)]];
        nowMovePoint = getCenterPositionWithHexaPoint(nearHexaPoint);
    }
    
}
- (void)clearNowMoveNode
{
    [nowMoveNode clear];
}




-(int)getVectorTypeByFrom:(CGPoint)fromHexaPoint To:(CGPoint)toHexaPoint
{
    CGPoint vec = ccp(toHexaPoint.x - fromHexaPoint.x, toHexaPoint.y - fromHexaPoint.y);
    int vectorSize = [self getVectorSizeFrom:fromHexaPoint To:toHexaPoint];
    CGPoint normalizedVec = ccp(vec.x / vectorSize, vec.y / vectorSize);
    
    for( int i = 0 ; i < 6; i ++)
    {
        if(dX[i] == normalizedVec.x
           && dY[i] == normalizedVec.y)
            return i;
    }
    return -1;
}
-(void)onMoveWithVectorType:(int)vectorType Length:(int)length Target:(Unit*)target isFirstMove:(bool)isFirstMove
{
    if(length <= 0)
    {
        [self finishMove];
        return;
    }
    
    Unit* firstGuy = nil;
    Unit* lastGuy = nil;
    int costLength = -1;
    for( int l = 1; l <= length; l++)
    {
        
        Unit* standGuy = [self getUnitInHexaPoint:ccp(target.hexaPosition.x + dX[vectorType] * l, target.hexaPosition.y + dY[vectorType] * l)];
        if(standGuy == nil && firstGuy == nil)
        {
            continue;
        }
        else if(standGuy == nil && firstGuy != nil)
            break;
        else{
            if(firstGuy == nil)
            {
                costLength = l;
                firstGuy = standGuy;
            }
            lastGuy = standGuy;
        }
    }
    
    if(firstGuy != nil)
    {
        target.hexaPosition = ccpAdd(target.hexaPosition, ccp(dX[vectorType] * (costLength - 1), dY[vectorType] * (costLength - 1)));
        
        // 애니메이션 여기서 추가해야해.
        AnimationNode aniNode;
        aniNode.node = target;
        aniNode.moveTo = getCenterPositionWithHexaPoint(target.hexaPosition);
        aniNode.type = MOVETO;
        animationQueue[animationCount] = aniNode;
        animationCount++;
        //target.position = getCenterPositionWithHexaPoint(target.hexaPosition);
        
        if(firstGuy.owner != target.owner)
        {
            [self applyDamageWithAttacker:target Target:firstGuy];
        }
        [self pushByPusher:target Target:lastGuy lastLength:(length-costLength)  isFirstPush:isFirstMove];
    }
    else
    {
        target.hexaPosition = ccpAdd(target.hexaPosition, ccp(dX[vectorType] * length, dY[vectorType] * length));
        
        // 애니메이션 여기서 추가해야해.
        AnimationNode aniNode;
        aniNode.node = target;
        aniNode.moveTo = getCenterPositionWithHexaPoint(target.hexaPosition);
        aniNode.type = MOVETO;
        animationQueue[animationCount] = aniNode;
        animationCount++;
        
        
        //target.position = getCenterPositionWithHexaPoint(target.hexaPosition);
        
        [self finishMove];
    }
}

-(void)applyDamageWithAttacker:(Unit*)attacker Target:(Unit*)target
{
    target.hp -= attacker.damage;
    attacker.hp -= target.damage;
    
    if(target.hp <= 0)
        [self thisGuyKiiled:target];
    
    if(attacker.hp <= 0)
        [self thisGuyKiiled:attacker];
}
-(void)pushByPusher:(Unit*)pusher Target:(Unit*)target lastLength:(int)lastLength isFirstPush:(bool)isFirstPush
{
    if(isFirstPush)
    {
        [self onMoveWithVectorType:[self getVectorTypeByFrom:pusher.hexaPosition To:target.hexaPosition] Length:pusher.damage - target.weight Target:target isFirstMove:NO];
    }
    else
        [self onMoveWithVectorType:[self getVectorTypeByFrom:pusher.hexaPosition To:target.hexaPosition] Length:lastLength - target.weight Target:target isFirstMove:NO];
}
-(void)finishMove
{
    // 애니메이션 할거있으면 하고, 여기서 touch 이벤트 받는거 다시 켜야혀~
    // 그 말은, 그 전에 터치 이벤트를 죽여야한다는거지!
    [self startAnimation];
}
-(void)thisGuyKiiled:(Unit*)unit
{
    //죽는 애니메이션 하셔~
    AnimationNode aniNode;
    aniNode.node = unit;
    aniNode.type = DIE;
    animationQueue[animationCount] = aniNode;
    animationCount++;
    
    
    //[self removeChild:unit cleanup:YES];
    //[unitList removeObject:unit];
}

-(int)getVectorSizeFrom:(CGPoint)fromHexaPoint To:(CGPoint)toHexaPoint
{
    CGPoint vec = ccp(toHexaPoint.x - fromHexaPoint.x, toHexaPoint.y - fromHexaPoint.y);
    if(abs(vec.x) > 0) return abs(vec.x);
    else return abs(vec.y);
}

-(void)startAnimation
{
    self.userInteractionEnabled = false;
    for(int i = 0 ; i < animationCount/2; i++)
    {
        AnimationNode temp = animationQueue[animationCount - i - 1];
        animationQueue[animationCount - i - 1] = animationQueue[i];
        animationQueue[i] = temp;
    }
    [self nextAnimation];
}
-(void)nextAnimation
{
    if(animationCount > 0)
    {
        animationCount--;
        
        AnimationNode node = animationQueue[animationCount];
        
        switch(node.type)
        {
            case MOVETO:
            {
                CCActionMoveTo* moveto = [CCActionMoveTo actionWithDuration:MOVE_DURATION
                                                                   position:node.moveTo];
                CCActionCallFunc* callfn = [CCActionCallFunc actionWithTarget:self selector:@selector(nextAnimation)];
                
                CCActionSequence* seq = [CCActionSequence actions:moveto, callfn, nil];
                [node.node runAction:seq];
            }break;
            case DIE:
            {
                CCActionScaleTo* scaleTo1 = [CCActionScaleTo actionWithDuration:DIE_DURATION scaleX:2.f scaleY:0.5f];
                CCActionScaleTo* scaleTo2 = [CCActionScaleTo actionWithDuration:DIE_DURATION scaleX:0.f scaleY:3.f];
                CCActionMoveTo* moveto = [CCActionMoveTo actionWithDuration:MOVE_DURATION
                                                                   position:getCenterPosition(MAP_WIDTH, MAP_HEIGHT)];
                Unit* unit = node.node;
                unit.hexaPosition = ccp(MAP_WIDTH, MAP_HEIGHT);
                CCActionCallFunc* callfn = [CCActionCallFunc actionWithTarget:self selector:@selector(nextAnimation)];
                
                
                CCActionSequence* seq = [CCActionSequence actions:scaleTo1, scaleTo2, moveto, callfn, nil];
                [node.node runAction:seq];
                //[self removeChild:node.node cleanup:YES];
                //[unitList removeObject:node.node];
            }break;
            case HORSEJUMP:
            {
                CCActionBezierTo* jump = [CCActionBezierTo actionWithDuration:JUMP_DURATION bezier:node.jumpConfig];
                CCActionCallFunc* callfn = [CCActionCallFunc actionWithTarget:self selector:@selector(nextAnimation)];
                CCActionSequence* seq = [CCActionSequence actions:jump, callfn, nil];
                [node.node runAction:seq];
            }
        }
        
    }
    else
        [self finishAnimation];
}


-(void)horeseJump:(Unit*)horse ToHexa:(CGPoint)hexa
{
    auto unitOnThere =[self getUnitInHexaPoint:hexa];
    if( unitOnThere != nil)
    {
        [self pushByPusher:horse Target:unitOnThere lastLength:-1  isFirstPush:YES];
    }

    CGPoint vec = getCenterPositionWithHexaPoint(hexa) - getCenterPositionWithHexaPoint(horse.hexaPosition);

    CGPoint middlePoint = getCenterPositionWithHexaPoint(horse.hexaPosition) + vec / 2.f
    + turnCGPointByRadian(vec/2.f, M_PI_2);
    
    
    ccBezierConfig bezier;
    bezier.endPosition = getCenterPositionWithHexaPoint(hexa);
    bezier.controlPoint_1 = bezier.controlPoint_2 = middlePoint;
    
    
    AnimationNode aniNode;
    aniNode.node = horse;
    aniNode.jumpConfig = bezier;
    aniNode.type = HORSEJUMP;
    animationQueue[animationCount] = aniNode;
    animationCount++;
    
    horse.hexaPosition = hexa;
    
    if( unitOnThere == nil)
    {
        [self finishMove];
    }
}
-(void)removeAnimationUnit
{
    Unit* unit = animationQueue[animationCount].node;
    [self removeChild:unit cleanup:YES];
    [unitList removeObject:unit];
}
-(void)finishAnimation
{
    self.userInteractionEnabled = true;
}
// -----------------------------------------------------------------------
#pragma mark - Button Callbacks
// -----------------------------------------------------------------------

- (void)onBackClicked:(id)sender
{
    // back to intro scene with transition
    //[[CCDirector sharedDirector] replaceScene:[IntroScene scene]
    //                           withTransition:[CCTransition transitionPushWithDirection:CCTransitionDirectionRight duration:1.0f]];
}

// -----------------------------------------------------------------------
@end
