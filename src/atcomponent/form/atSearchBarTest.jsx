import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtSearchBar } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtSearchBarTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { value: '' }
    }

    handleChange(value) {
        console.log("value:", value)
        this.setState({ value: value })
    }

    handleConfirm() {
        console.log("search...")
    }

    handleClear() {
        console.log("clear...")
        this.setState({ value: '' })
    }

    render() {
        return (
            <View>
                <AtSearchBar
                    placeholder='请输入内容...'
                    actionName='搜索'
                    value={this.state.value}
                    onChange={this.handleChange.bind(this)}
                    onConfirm={this.handleConfirm.bind(this)}
                    onActionClick={this.handleConfirm.bind(this)}
                    onClear={this.handleClear.bind(this)}
                />
            </View>
        )
    }
}