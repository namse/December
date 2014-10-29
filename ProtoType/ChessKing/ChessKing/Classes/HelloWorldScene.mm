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
float CGPointSize(const CGPoint&p1, const CGPoint&p2)
{
    return sqrtf(powf(p1.x - p2.x, 2) + powf(p1.y - p2.y, 2));
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
int getVectorSize(CGPoint fromHexaPoint, CGPoint toHexaPoint)
{
    CGPoint vec = ccp(toHexaPoint.x - fromHexaPoint.x, toHexaPoint.y - fromHexaPoint.y);
    if(abs(vec.x) > 0) return abs(vec.x);
    else return abs(vec.y);
}
int getVectorTypeWithHexaPoints(CGPoint fromHexaPoint, CGPoint toHexaPoint)
{
    CGPoint vec = ccp(toHexaPoint.x - fromHexaPoint.x, toHexaPoint.y - fromHexaPoint.y);
    int vectorSize = getVectorSize(fromHexaPoint, toHexaPoint);
    CGPoint normalizedVec = ccp(vec.x / vectorSize, vec.y / vectorSize);
    
    for( int i = 0 ; i < 6; i ++)
    {
        if(dX[i] == normalizedVec.x
           && dY[i] == normalizedVec.y)
            return i;
    }
    return -1;
}
int getVectorTypeWithScreenPoints(CGPoint fromPoint, CGPoint toPoint)
{
    CGPoint vec = toPoint - fromPoint;
    int ret = -1;
    float maxCeta = -INFINITY;
    for( int i = 0 ; i < 6; i++)
    {
        float dotProd = vec.x * dX[i] + vec.y * dY[i];
        float cosCeta = dotProd /
        (sqrtf(powf(vec.x, 2) + powf(vec.y, 2)) * sqrtf(powf(dX[i], 2) + powf(dY[i], 2)));
        if(maxCeta < cosCeta)
        {
            ret = i;
            maxCeta = cosCeta;
        }
    }
    return ret;
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
    freedomMoveQueue = [NSMutableArray array];
    turnCount = 0;
    isFirstTurn = true;
    
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
    
    if(( fabs( ( double )[ [ UIScreen mainScreen ] bounds ].size.height - ( double )568 ) < DBL_EPSILON ))
    {
        [self setScale:0.45f];
    }
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
        if(pickedUnit.unitType == BISHOP)
        {
            //pickedUnit.position = getCenterPositionWithHexaPoint(getHexaPointByScreenPoint(touchLoc));
            [freedomMoveQueue removeAllObjects];
            [freedomMoveQueue addObject:[NSValue valueWithCGPoint:getHexaPointByScreenPoint(touchLoc)]];
            [self showMovableAreaWithFreeMove:pickedUnit MoveQueue:freedomMoveQueue];
        }
        else
        {
            pickedUnit.position = ccp( (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * 9.f + touchLoc.x) / 10.f,
                                      (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * 9.f + touchLoc.y) / 10.f);
            
            [self showMovableArea:touchedHexaPoint velocity:pickedUnit.velocity];
        }
    }

}

-(void)touchMoved:(UITouch *)touch withEvent:(UIEvent *)event
{
    CGPoint touchLoc = [touch locationInNode:self];
    
    if(isPicked == true && pickedUnit != nil)
    {
        
        if(pickedUnit.unitType == BISHOP)
        {
            if(CGPointEqualToPoint(getHexaPointByScreenPoint(touchLoc), pickedUnit.hexaPosition))
            {
                [freedomMoveQueue removeAllObjects];
                [freedomMoveQueue addObject:[NSValue valueWithCGPoint:getHexaPointByScreenPoint(touchLoc)]];
            }
            else
            {
                
                bool didGoThere = false;
                bool ableGoThere = false;
                for(NSValue* value in freedomMoveQueue)
                {
                    CGPoint point = [value CGPointValue];
                    if(CGPointEqualToPoint(point, getHexaPointByScreenPoint(touchLoc)))
                    {
                        didGoThere = true;
                        break;
                    }
                }
                
                for(NSValue* value in moveableAreaList)
                {
                    CGPoint point = [value CGPointValue];
                    if(CGPointEqualToPoint(point, getHexaPointByScreenPoint(touchLoc)))
                    {
                        for( int i = 0 ; i < 6 ; i ++)
                        {
                            CGPoint dPoint = getHexaPointByScreenPoint(touchLoc) + ccp(dX[i], dY[i]);
                            if(CGPointEqualToPoint(dPoint, [[freedomMoveQueue lastObject]CGPointValue]))
                            {
                                ableGoThere = true;
                                break;
                            }
                        }
                        if(ableGoThere == true)
                            break;
                    }
                }
                if(ableGoThere == true && didGoThere == NO && [freedomMoveQueue count] <= pickedUnit.velocity)
                {
                    [freedomMoveQueue addObject:[NSValue valueWithCGPoint:getHexaPointByScreenPoint(touchLoc)]];
                }
                else if(didGoThere == true)
                {
                    if([freedomMoveQueue count] > 1)
                    {
                        CGPoint point = [[freedomMoveQueue objectAtIndex:[freedomMoveQueue indexOfObject:[freedomMoveQueue lastObject]]-1]CGPointValue];
                        if(CGPointEqualToPoint(point, getHexaPointByScreenPoint(touchLoc)))
                        {
                            [freedomMoveQueue removeLastObject];
                        }
                    }
                }
            }
            
            //pickedUnit.position = getCenterPositionWithHexaPoint([[freedomMoveQueue lastObject]CGPointValue]);
            
            [self showMovableAreaWithFreeMove:pickedUnit MoveQueue:freedomMoveQueue];
            [self showNowMoveNode];
            
        }
        else
        {
            pickedUnit.position = ccp( (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * 9.f + touchLoc.x) / 10.f,
                                      (getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * 9.f + touchLoc.y) / 10.f);
            nowMovePoint = ccp( getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).x * (1.f+PULL_RATIO) - touchLoc.x * PULL_RATIO ,
                               getCenterPositionWithHexaPoint(pickedUnit.hexaPosition).y * (1.f+PULL_RATIO) - touchLoc.y * PULL_RATIO);
        
            // 아래 이거 쓸모있는건가?
            if(CGPointEqualToPoint(getHexaPointByScreenPoint(touchLoc), pickedUnit.hexaPosition) )
                ;
            else
                [self showNowMoveNode];
        }
        
        
        
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
            if(pickedUnit.unitType == BISHOP)
            {
                [self freeMove:pickedUnit MoveQueue:freedomMoveQueue];
            }
            else if (pickedUnit.unitType == HORSE) {
                [self horeseJump:pickedUnit ToHexa:getHexaPointByScreenPoint(nowMovePoint)];
            }
            else
            {
                [self onMoveWithVectorType:getVectorTypeWithHexaPoints(pickedUnit.hexaPosition,getHexaPointByScreenPoint(nowMovePoint))
                                    Length:getVectorSize(pickedUnit.hexaPosition, getHexaPointByScreenPoint(nowMovePoint))
                                    Target:pickedUnit isFirstMove:YES];
                
            }
            turnCount++;
            
            bool isNearAlkaStone = false;
            for(Unit* unit in unitList)
            {
                if(unit.owner == pickedUnit.owner)
                {
                    for(int i = 0; i < 6; i++)
                    {
                        if(CGPointEqualToPoint(alkaStone.hexaPosition - unit.hexaPosition, ccp(dX[i], dY[i])))
                        {
                            isNearAlkaStone = true;
                        }
                    }
                }
            }
            
            
            int maxturn = MAX_TURN_COUNT;
            if(isNearAlkaStone == true)
            {
                maxturn++;
            }
            if(isFirstTurn == true)
            {
                maxturn = MAX_TURN_COUNT;
            }
            if(turnCount < maxturn)
            {
                ;
            }
            else
            {
                isFirstTurn = false;
                turnCount = 0;
                if(whoTurn == PLAYER_1)
                {
                    whoTurn = PLAYER_2;
                }
                else whoTurn = PLAYER_1;
            }
        }
    }
    [self clearMovablePosition];
    [self clearNowMoveNode];
}



- (void)showMovableAreaWithFreeMove:(Unit*)target MoveQueue:(NSMutableArray*)moveQueue
{
    [self clearMovablePosition];
    
    //Check now is last
    {
        CGPoint point = [[moveQueue lastObject]CGPointValue];
        auto unitOnThere = [self getUnitInHexaPoint:point];
        if(unitOnThere != nil && unitOnThere != target)
        {
            return;
        }
    }
    
    CGFloat dX[6] = {-1, 0, 1, 1, 0, -1,};
    CGFloat dY[6] = {-1, -1, 0, 1, 1, 0,};
    //BFS
    CGPoint buf[1024];
    ccColor4B color[1024];
    BOOL isLast[1024];
    memset(isLast, NO, sizeof(isLast));
    int head, tail, newTail;
    head = tail = newTail = 0;
    buf[0] = [[moveQueue lastObject]CGPointValue];
    tail = newTail = 1;
    for(int v = 1; v <= target.velocity - [moveQueue count] + 1; v++)
    {
        for(int i = head ; i < tail; i++)
        {
            if(isLast[i])
                continue;
            for( int a = 0 ; a < 6; a++) // index for Angle
            {
                CGPoint point = buf[i] + ccp(dX[a], dY[a]);
                bool isEqual = false;
                for (NSValue *value in moveQueue) {
                    auto gonePoint = [value CGPointValue];
                    if(CGPointEqualToPoint(gonePoint, point))
                    {
                        isEqual = true;
                    }
                }
                if(isEqual)
                    continue;
                if([self isInside:point] == NO)
                {
                    buf[newTail] = point;
                    color[newTail] = MOVABLE_AREA_COLOR_OUTSIDE;
                    isLast[newTail] = YES;
                    newTail ++;
                }
                Unit* unit = [self getUnitInHexaPoint:point];
                if(unit != nil)
                {
                    if(unit.owner == pickedUnit.owner)
                    {
                        // ALIY
                        buf[newTail] = point;
                        color[newTail] = MOVABLE_AREA_COLOR_ALIY;
                        isLast[newTail] = YES;
                        newTail ++;
                    }
                    else
                    {
                        //ENEMY
                        buf[newTail] = point;
                        color[newTail] = MOVABLE_AREA_COLOR_ENEMY;
                        isLast[newTail] = YES;
                        newTail ++;
                    }
                }
                else
                {
                    buf[newTail] = point;
                    color[newTail] = MOVABLE_AREA_COLOR_NORMAL;
                    isLast[newTail] = NO;
                    newTail ++;
                }
            }
        }
        head = tail;
        tail = newTail;
    }
    
    for(int i = 1; i < newTail; i++)
    {
        [movableFocusArea drawDot:getCenterPositionWithHexaPoint(buf[i]) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:color[i]]];
        [moveableAreaList addObject:[NSValue valueWithCGPoint:buf[i]]];
    }
}
- (void)showMovableArea:(CGPoint)startHexaPoint velocity:(int)velocity
{
    [self clearMovablePosition];
    CGFloat dX[6] = {-1, 0, 1, 1, 0, -1,};
    CGFloat dY[6] = {-1, -1, 0, 1, 1, 0,};
    
    
    if(pickedUnit.unitType == BISHOP)
    {
        //NO!
    }
    else
    {
        
        for( int i = 0 ; i < 6; i++) // index for Angle
        {
            for( int v = 1 ; v <= velocity; v++)
            {
                CGPoint point = ccp( startHexaPoint.x + dX[i] * v,
                                    startHexaPoint.y + dY[i] * v);
                
                if([self isInside:point] == NO)
                {
                    //last point for self-die
                    [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:MOVABLE_AREA_COLOR_OUTSIDE]];
                    [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                    break;
                }
                Unit* unit = [self getUnitInHexaPoint:point];
                if(unit != nil)
                {
                    if(unit.owner == pickedUnit.owner)
                    {
                        // ALIY
                        [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:MOVABLE_AREA_COLOR_ALIY]];
                        [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                        if(pickedUnit.unitType == HORSE)
                            continue;
                        else
                            break;
                    }
                    else
                    {
                        //ENEMY
                        [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:MOVABLE_AREA_COLOR_ENEMY]];
                        [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                        break;
                    }
                }
                else
                {
                    [movableFocusArea drawDot:getCenterPositionWithHexaPoint(point) radius:MOVABLE_RAIDUS color:[CCColor colorWithCcColor4b:MOVABLE_AREA_COLOR_NORMAL]];
                    [moveableAreaList addObject:[NSValue valueWithCGPoint:point]];
                }
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
    int maxX = 7;
    int maxY = 10;
    int startX = 2;
    int startY = 3;
    for( int x = 0 ; x < maxX; x ++)
    {
        for( int y = 0 ; y < maxY - (x%2) ; y++)
        {
            CGPoint point =  ccp(startX + x,
                                 startY + y + ((x+1)/2));
            [mapNodeList addObject:[NSValue valueWithCGPoint:point]];
        }
    }
    self.position = self.position;
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

-(bool)isInside:(CGPoint)hexaPoint
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
            return true;
        }
    }
    return false;
}

-(void)generateUnitByHardCoding
{
    
    //Player_1
    {
        Unit* pawn1 = [Unit UnitWithName:@"P" unitType:PAWN HP:30 Damage:3 Velocity:2 weight:0 owner:PLAYER_1 HexaPosition:ccp(4,7)];
        [self addChild:pawn1];
        [unitList addObject:pawn1];
        
        Unit* pawn2 = [Unit UnitWithName:@"P" unitType:PAWN HP:30 Damage:3 Velocity:2 weight:0 owner:PLAYER_1 HexaPosition:ccp(6,8)];
        [self addChild:pawn2];
        [unitList addObject:pawn2];
        
        Unit* horse1 = [Unit UnitWithName:@"H" unitType:HORSE HP:30 Damage:3 Velocity:4 weight:0 owner:PLAYER_1 HexaPosition:ccp(4,6)];
        [self addChild:horse1];
        [unitList addObject:horse1];
        
        Unit* horse2 = [Unit UnitWithName:@"H" unitType:HORSE HP:30 Damage:3 Velocity:4 weight:0 owner:PLAYER_1 HexaPosition:ccp(6,7)];
        [self addChild:horse2];
        [unitList addObject:horse2];
        
        Unit* bishop = [Unit UnitWithName:@"B" unitType:BISHOP HP:30 Damage:2 Velocity:3 weight:0 owner:PLAYER_1 HexaPosition:ccp(5,7)];
        [self addChild:bishop];
        [unitList addObject:bishop];
        
        Unit* king = [Unit UnitWithName:@"K" unitType:KING HP:30 Damage:3 Velocity:3 weight:1 owner:PLAYER_1 HexaPosition:ccp(5,6)];
        [self addChild:king];
        [unitList addObject:king];
    }
    
    
    //Player_2
    {
        Unit* pawn1 = [Unit UnitWithName:@"P" unitType:PAWN HP:30 Damage:3 Velocity:2 weight:0 owner:PLAYER_2 HexaPosition:ccp(4,10)];
        [self addChild:pawn1];
        [unitList addObject:pawn1];
        
        Unit* pawn2 = [Unit UnitWithName:@"P" unitType:PAWN HP:30 Damage:3 Velocity:2 weight:0 owner:PLAYER_2 HexaPosition:ccp(6,11)];
        [self addChild:pawn2];
        [unitList addObject:pawn2];
        
        Unit* horse1 = [Unit UnitWithName:@"H" unitType:HORSE HP:30 Damage:3 Velocity:4 weight:0 owner:PLAYER_2 HexaPosition:ccp(4,11)];
        [self addChild:horse1];
        [unitList addObject:horse1];
        
        Unit* horse2 = [Unit UnitWithName:@"H" unitType:HORSE HP:30 Damage:3 Velocity:4 weight:0 owner:PLAYER_2 HexaPosition:ccp(6,12)];
        [self addChild:horse2];
        [unitList addObject:horse2];
        
        Unit* bishop = [Unit UnitWithName:@"B" unitType:BISHOP HP:30 Damage:2 Velocity:3 weight:0 owner:PLAYER_2 HexaPosition:ccp(5,11)];
        [self addChild:bishop];
        [unitList addObject:bishop];
        
        Unit* king = [Unit UnitWithName:@"K" unitType:KING HP:30 Damage:3 Velocity:3 weight:1 owner:PLAYER_2 HexaPosition:ccp(5,12)];
        [self addChild:king];
        [unitList addObject:king];
    }
    
    alkaStone = [Unit UnitWithName:@"A" unitType:NPC HP:20 Damage:6 Velocity:0 weight:1 owner:PLAYER_NPC HexaPosition:ccp(5, 9)];
    [self addChild:alkaStone];
    [unitList addObject:alkaStone];
    
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
    
    if(pickedUnit.unitType == BISHOP)
    {
        for( int i = 1 ; i < [freedomMoveQueue count]; i++)
        {
            CGPoint pointFrom = getCenterPositionWithHexaPoint([[freedomMoveQueue objectAtIndex:i-1]CGPointValue]);
            CGPoint pointTo = getCenterPositionWithHexaPoint([[freedomMoveQueue objectAtIndex:i]CGPointValue]);
            [nowMoveNode drawSegmentFrom:pointFrom to:pointTo radius:BISHOP_PAST_RADIUS color:[CCColor colorWithCcColor4b:PAST_COLOR_BISHOP_ALREADY]];
        }
    }
    else
    {
        CGPoint moveHexaPoint = getHexaPointByScreenPoint(nowMovePoint);
        if([self isEnableMovePosition:moveHexaPoint])
        {
            [nowMoveNode drawDot:getCenterPositionWithHexaPoint(getHexaPointByScreenPoint(nowMovePoint)) radius:MOVABLE_RAIDUS*1.5f color:[CCColor colorWithCcColor3b:ccc3(255, 0, 0)]];
        }
        else
        {
            //Find Near Point
            CGPoint nearHexaPoint = pickedUnit.hexaPosition;
            int vecType = getVectorTypeWithScreenPoints(nowMovePoint, getCenterPositionWithHexaPoint(pickedUnit.hexaPosition));
            NSLog(@"vecType : %d",vecType);
            while(1)
            {
                CGPoint point = nearHexaPoint - CGPointMake(dX[vecType], dY[vecType]);
                if([self isEnableMovePosition:point] && CGPointSize(point, pickedUnit.hexaPosition) < CGPointSize(moveHexaPoint, pickedUnit.hexaPosition))
                {
                    nearHexaPoint = point;
                    continue;
                }
                else
                    break;
            }
//            for(NSValue *value in moveableAreaList)
//            {
//                CGPoint point = [value CGPointValue];
//                if( min > powf((point.x - moveHexaPoint.x), 2) + powf((point.y - moveHexaPoint.y), 2) )
//                {
//                    min = powf((point.x - moveHexaPoint.x), 2) + powf((point.y - moveHexaPoint.y), 2);
//                    nearHexaPoint = point;
//                }
//            }
            [nowMoveNode drawDot:getCenterPositionWithHexaPoint(nearHexaPoint) radius:MOVABLE_RAIDUS*1.5f color:[CCColor colorWithCcColor3b:ccc3(255, 0, 0)]];
            nowMovePoint = getCenterPositionWithHexaPoint(nearHexaPoint);
        }
    }
}
- (void)clearNowMoveNode
{
    [nowMoveNode clear];
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
        aniNode.duration = MOVE_DURATION;
        animationQueue[animationCount] = aniNode;
        animationCount++;
        
        if([self isInside:target.hexaPosition] == NO)
        {
            [self thisGuyKiiled:target];
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
        aniNode.duration = MOVE_DURATION;
        animationQueue[animationCount] = aniNode;
        animationCount++;
        
        if([self isInside:target.hexaPosition] == NO)
        {
            [self thisGuyKiiled:target];
        }
        
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
        [self onMoveWithVectorType:getVectorTypeWithHexaPoints(pusher.hexaPosition, target.hexaPosition) Length:pusher.damage - target.weight Target:target isFirstMove:NO];
    }
    else
        [self onMoveWithVectorType:getVectorTypeWithHexaPoints(pusher.hexaPosition, target.hexaPosition) Length:lastLength - target.weight Target:target isFirstMove:NO];
    if(pusher.owner != target.owner)
    {
        [self applyDamageWithAttacker:target Target:pusher];
    }
}
-(void)finishMove
{
    // 애니메이션 할거있으면 하고, 여기서 touch 이벤트 받는거 다시 켜야혀~
    // 그 말은, 그 전에 터치 이벤트를 죽여야한다는거지!
    [self startAnimation];
}
-(void)thisGuyKiiled:(Unit*)unit
{
    if(unit.isAlive == true)
    {
        unit.isAlive = false;
        //죽는 애니메이션 하셔~
        AnimationNode aniNode;
        aniNode.node = unit;
        aniNode.type = DIE;
        animationQueue[animationCount] = aniNode;
        animationCount++;
        
        
        //[self removeChild:unit cleanup:YES];
        //[unitList removeObject:unit];
    }
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
        int vecType = getVectorTypeWithHexaPoints(horse.hexaPosition, unitOnThere.hexaPosition);
        hexa = hexa -ccp(dX[vecType], dY[vecType]);
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

    
    if([self isInside:horse.hexaPosition] == NO)
    {
        [self thisGuyKiiled:horse];
        [self finishMove];
    }
    else if( unitOnThere == nil)
    {
        [self finishMove];
    }
    else
    {
        [self pushByPusher:horse Target:unitOnThere lastLength:-1  isFirstPush:YES];
    }
}

-(void)freeMove:(Unit*)target MoveQueue:(NSMutableArray*)moveQueue
{
    float divideNumber;
   
    auto unitOnThere = [self getUnitInHexaPoint:[[freedomMoveQueue lastObject]CGPointValue]];
    if(unitOnThere != nil)
    {
        divideNumber = [freedomMoveQueue count] - 2;
    }
    else
    {
        divideNumber = [freedomMoveQueue count] - 1;
    }
    for(int i = 1; i < [freedomMoveQueue count]; i++)
    {
        NSValue* value = [freedomMoveQueue objectAtIndex:i];
        CGPoint point = [value CGPointValue];
        if(i == [freedomMoveQueue count] -1)
        {
            if(unitOnThere != nil)
            {
                target.hexaPosition = [[freedomMoveQueue objectAtIndex:i-1]CGPointValue];
                [self pushByPusher:target Target:unitOnThere lastLength:-1  isFirstPush:YES];
                break;
            }
            else
            {
                target.hexaPosition = [[freedomMoveQueue lastObject]CGPointValue];
            }
            
        }
        AnimationNode aniNode;
        aniNode.node = target;
        aniNode.moveTo = getCenterPositionWithHexaPoint(point);
        aniNode.type = MOVETO;
        aniNode.duration = MOVE_DURATION / divideNumber;
        animationQueue[animationCount] = aniNode;
        animationCount++;
    }
    
    if([self isInside:target.hexaPosition] == NO)
    {
        [self thisGuyKiiled:target];
        [self finishMove];
    }
    else if( unitOnThere == nil)
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
