import Taro, { Component } from '@tarojs/taro'
import { View, Swiper, SwiperItem } from '@tarojs/components'

export default class SwiperTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>SwiperTest</Text>
                <Swiper
                    className='test-h'
                    indicatorColo='#999'
                    indicatorColor='#333'
                    vertical
                    circular
                    indicatorDots
                    autoplay
                >
                    <SwiperItem className='demo-text-1'>1</SwiperItem>
                    <SwiperItem className='demo-text-2'>2</SwiperItem>
                    <SwiperItem className='demo-text-3'>3</SwiperItem>
                </Swiper>
            </View>
        )
    }
}