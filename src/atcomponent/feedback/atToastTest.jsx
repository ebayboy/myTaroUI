
/* Toast 情提示: 
提供一个简单的 Toast，可以传入 icon、image 来展示不同的效果，另外提供日常使用频繁的三种状态来快速实现效果，页面层级为1090 */

import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtToast, AtButton } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtToastTest.scss';

const INIT_STATE = {
    text: '',
    icon: '',
    image: '',
    status: '',
    hasMask: false,
    isOpened: false,
    duration: 3000
}

export default class AtToastTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            ...INIT_STATE
        }
    }

    handlerClickToast() {
        console.log("click Toast...")
        this.setState({ isOpened: false })
    }

    handlerClickBtn(text, icon, image, status, hasMask) {
        console.log("before click:", text, icon, image, status, hasMask)

        if (this.state.isOpened) {
            console.log("internel return!!!")
            return this.setState(INIT_STATE)
        }

        //set state
        const state = Object.assign(
            { ...INIT_STATE, isOpened: true },
            { text: text, icon: icon, image: image, status: status, hasMask: hasMask }
        )

        this.setState(state)

        console.log("after click:", state)
    }

    handlerCloseToast() {
        console.log("close Toast...")
        this.setState({ isOpened: false })
    }

    render() {
        const {
            text,
            icon,
            image,
            status,
            isOpened,
            duration,
            hasMask
        } = this.state

        return (
            <View>
                <View> <AtButton onClick={this.handlerClickBtn.bind(this, 'text+icon', 'start', '', '', false)}>text + icon</AtButton> </View>
                <View> <AtButton onClick={this.handlerClickBtn.bind(this, 'image', '', 'http://storage.360buyimg.com/mtd/home/group-21533885306540.png', '', false)}>image</AtButton> </View>
                <View> <AtButton onClick={this.handlerClickBtn.bind(this, 'text+mask+statue', '', '', 'loading', true)}>hasmask</AtButton> </View>
                <AtToast
                    text={text}
                    icon={icon}
                    image={image}
                    status={status}
                    isOpened={isOpened}
                    duration={duration}
                    hasMask={hasMask}
                    onClick={this.handlerClickToast.bind(this)}
                    onClose={this.handlerCloseToast.bind(this)}
                />
            </View>
        )
    }
}