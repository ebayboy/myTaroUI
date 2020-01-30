import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtNavBar } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtNavBarTest extends Component {
    constructor() {
        super(...arguments)
    }
    onClickLeftIcon() {
        console.log("onClickLeftIcon")
    }
    onClickRgIconNd() {
        console.log("onClickRgIconNd")
    }
    onClickRgIconSt() {
        console.log("onClickRgIconSt")
    }

    render() {
        return (
            <View>
                <AtNavBar
                    onClickRgIconSt={this.handleClick}
                    onClickRgIconNd={this.handleClick}
                    onClickLeftIcon={this.handleClick}
                    color='#000'
                    leftText='返回'
                    rightFirstIconType='bullet-list'
                    rightSecondIconType='user'
                    onClickLeftIcon={this.onClickLeftIcon}
                    onClickRgIconNd={this.onClickRgIconNd}
                    onClickRgIconSt={this.onClickRgIconSt}
                >
                    <View>Taro UI</View>
                </AtNavBar>
            </View>
        )
    }
}