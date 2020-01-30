import Taro, { Component, setInnerAudioOption } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtSegmentedControl } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtSegmentedControlTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 0 }
    }

    handleClick(value) {
        this.setState({ current: value })
    }

    render() {
        return (
            <View>
                <AtSegmentedControl
                    current={this.state.current}
                    values={['标签1', '标签2', '标签3']}
                    onClick={this.handleClick.bind(this)}
                    selectedColor='#FF4949'
                    fontSize='30'
                ></AtSegmentedControl>
                {
                    this.state.current === 0 ? <View className='tab-content'>标签1的内容</View> : null
                }
                {
                    this.state.current === 1 ? <View className='tab-content'>标签2的内容</View> : null
                }
                {
                    this.state.current === 2 ? <View className='tab-content'>标签3的内容</View> : null
                }
            </View>
        )
    }
}