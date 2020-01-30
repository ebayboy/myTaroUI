import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtCard } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtCardTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <View>Card一般用法</View>
                <AtCard
                    note='小Tips'
                    extra='额外信息'
                    title='这是个标题'
                    thumb='http://www.logoquan.com/upload/list/20180421/logoquan15259400209.PNG'
                >这也是内容区 可以随意定义功能</AtCard>
                <View>card 带图标</View>
                <AtCard
                    isFull={false}
                    title='这是个标题'
                    icon={{ value: 'tags', color: '#77a1fd' }}
                > 这也是内容区 可以随意定义功能 </AtCard>
            </View>
        )
    }
}