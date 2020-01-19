import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtTag } from 'taro-ui'

//import './atTagTest.scss'
import 'taro-ui/dist/style/index.scss' // 全局引入一次即可

export default class AtTagTest extends Component {

    constructor() {
        super(...arguments)
        this.state = {
            active: 'true'
        }
    }

    tagOnclick() {
        this.setState({
            active: !this.state.active
        })
    }

    render() {
        return (
            <View>
                <Text>base tag</Text>
                <AtTag >标签</AtTag>
                <AtTag type='primary' circle>primary circle tag</AtTag>
                <AtTag size='small'>small tag</AtTag>
                <AtTag active={this.state.active} onClick={this.tagOnclick.bind(this)}>click tag</AtTag>
            </View>
        )

    }
}