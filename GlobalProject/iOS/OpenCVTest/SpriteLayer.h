
#import <QuartzCore/QuartzCore.h>

@interface SpriteLayer : CALayer

@property (readwrite, nonatomic) NSUInteger spriteIndex;

+ (id)layerWithImage:(CGImageRef)image spriteSize:(CGSize)size;

- (NSUInteger)currentSpriteIndex;

@end
