import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtList, AtListItem } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtListTestNew extends Component {
    constructor() {
        super(...arguments)
    }

    handleClick(e) {
        console.log("e:", e)
    }

    handleChange(e) {
        console.log("e", e)
    }

    render() {
        return (
            <View>
                <View>List一般用法</View>
                <AtList>
                    <AtListItem title='标题文字' onClick={this.handleClick} />
                    <AtListItem title='标题文字' arrow='right' />
                    <AtListItem title='标题文字' extraText='详细信息' />
                    <AtListItem title='禁用状态' disabled extraText='详细信息' />
                </AtList>
                <View>List带描述信息</View>
                <AtList>
                    <AtListItem title='标题文字' note='描述信息' />
                    <AtListItem title='标题文字' note='描述信息' arrow='right' />
                    <AtListItem
                        title='标题文字'
                        note='描述信息'
                        arrow='right'
                        extraText='详细信息'
                    />
                </AtList>
                <View>包含图片</View>
                <AtList>
                    <AtListItem
                        title='标题文字'
                        arrow='right'
                        thumb='https://img12.360buyimg.com/jdphoto/s72x72_jfs/t6160/14/2008729947/2754/7d512a86/595c3aeeNa89ddf71.png'
                    />
                    <AtListItem
                        title='标题文字'
                        note='描述信息'
                        arrow='right'
                        thumb='http://img10.360buyimg.com/jdphoto/s72x72_jfs/t5872/209/5240187906/2872/8fa98cd/595c3b2aN4155b931.png'
                    />
                    <AtListItem
                        title='标题文字'
                        note='描述信息'
                        extraText='详细信息'
                        arrow='right'
                        thumb='http://img12.360buyimg.com/jdphoto/s72x72_jfs/t10660/330/203667368/1672/801735d7/59c85643N31e68303.png'
                    />
                </AtList>
                <View>支持图标的 Item</View>
                <AtList>
                    <AtListItem title='标题文字' note='描述信息' arrow='right'
                        iconInfo={{ size: 25, color: '#78A4FA', value: 'calendar', }}
                    />
                    <AtListItem title='标题文字'
                        note='描述信息' extraText='详细信息' arrow='right'
                        iconInfo={{ size: 25, color: '#FF4949', value: 'bookmark', }}
                    />
                </AtList>
                <View>无边框的 Item</View>
                <AtList hasBorder={false}>
                    <AtListItem
                        id='item1'
                        isSwitch
                        title='标题文字'
                        hasBorder={false}
                        onSwitchChange={this.handleChange}
                    />
                    <AtListItem
                        id='item2'
                        switchIsCheck
                        isSwitch
                        title='标题文字'
                        hasBorder={false}
                        onSwitchChange={this.handleChange}
                        onClick = {this.handleClick}
                    />
                </AtList>
            </View>
        )
    }
}