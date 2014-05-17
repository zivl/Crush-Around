//
//  NotificationView.h
//  DestroyAroundMe
//
//  Created by Ziv Levy on 5/5/14.
//  Copyright (c) 2014 Ziv Levy. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NotificationView : UIView

@property (nonatomic, weak) IBOutlet UIImageView *bg;
@property (nonatomic, weak) IBOutlet UILabel *message;
@property (nonatomic, strong) IBOutlet UIButton *okButton;

-(void)showNotificationWithMessage:(NSString *)iMessgae;
-(void)showNotificationWithMessage:(NSString *)iMessgae withTarget:(id)target withAction:(SEL)action;
-(void)removeHandlerWithTarget:(id)target withAction:(SEL)action;
-(void)hideNotificationMessage;

-(IBAction)onOKButtonPressed:(id)sender;
@end
